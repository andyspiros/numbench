#ifndef ACTION_PARALLEL_CHOLESKY_HH_
#define ACTION_PARALLEL_CHOLESKY_HH_

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

#include "lapack_interface.hh"
#include "STL_interface.hh"

#include <string>

template<class Interface>
class Action_parallel_cholesky {
  typedef lapack_interface<typename Interface::real_type> LapackInterface;

public :

  // Constructor
  BTL_DONT_INLINE Action_parallel_cholesky( int size ) : _size(size)
  {
    MESSAGE("Action_parallel_cholesky Ctor");

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
        typename LapackInterface::stl_matrix temp_stl;
        init_matrix_symm<pseudo_random>(temp_stl, size);
        Global_A_stl.reserve(size*size);
        const double add = 5000./size;
        for (int r = 0; r < size; ++r)
          for (int c = 0; c < size; ++c)
            if (r==c)
              Global_A_stl.push_back((std::abs(temp_stl[r][c])+add)*size);
            else
              Global_A_stl.push_back(temp_stl[r][c]);
    }

    Interface::scatter_matrix(Global_A_stl, Local_A_stl, desc, size, size, 64, 64);
    LocalRows = desc[8];
    LocalCols = Local_A_stl.size()/desc[8];

    // Generic local matrix and vectors initialization
    Interface::matrix_from_stl(Local_A_ref, Local_A_stl);
    Interface::matrix_from_stl(Local_A    , Local_A_stl);

    _cost = 0;
    for (int j=0; j<_size; ++j) {
      double r = std::max(_size - j -1,0);
      _cost += 2*(r*j+r+j);
    }
  }


  // Invalidate copy constructor
  Action_parallel_cholesky(const Action_parallel_cholesky&)
  {
    INFOS("illegal call to Action_parallel_cholesky copy constructor");
    exit(1);
  }

  // Destructor
  ~Action_parallel_cholesky()
  {
    MESSAGE("Action_parallel_cholesky destructor");

    // Deallocation
    Interface::free_matrix(Local_A_ref, Local_A_stl.size());
    Interface::free_matrix(Local_A    , Local_A_stl.size());
  }

  // Action name
  static inline std::string name()
  {
    return "cholesky_" + Interface::name();
  }

  double nb_op_base()
  {
    return _cost;
  }

  BTL_DONT_INLINE void initialize()
  {
    Interface::copy_matrix(Local_A_ref, Local_A, Local_A_stl.size());
  }

  BTL_DONT_INLINE void calculate()
  {
    Interface::parallel_cholesky(Local_A, desc);
  }

  BTL_DONT_INLINE void check_result()
  {
  }


private:
  int _size, desc[9], LocalRows, LocalCols;
  double _cost;
  bool iamroot;

  typename Interface::stl_matrix Global_A_stl;
  typename Interface::stl_matrix Local_A_stl;
  typename Interface::gene_matrix Local_A_ref;
  typename Interface::gene_matrix Local_A;
};

#endif /* ACTION_PARALLEL_CHOLESKY_HH_ */
