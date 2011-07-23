#ifndef ACTION_PARALLEL_LU_DECOMP_HH_
#define ACTION_PARALLEL_LU_DECOMP_HH_

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

#include "lapack_interface.hh"
#include "STL_interface.hh"

#include <string>

template<class Interface>
class Action_parallel_lu_decomp {

public :

  // Constructor
  BTL_DONT_INLINE Action_parallel_lu_decomp( int size ) : _size(size)
  {
    MESSAGE("Action_parallel_lu_decomp Ctor");

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
      init_vector<pseudo_random>(Global_A_stl, size*size);
    }

    const int blocksize = std::max(std::min(size/4, 64), 2);
    Interface::scatter_matrix(Global_A_stl, Local_A_stl, desc, size, size, blocksize, blocksize);
    LocalRows = desc[8];
    LocalCols = Local_A_stl.size()/desc[8];

    // Generic local matrix and vectors initialization
    Interface::matrix_from_stl(Local_A_ref, Local_A_stl);
    Interface::matrix_from_stl(Local_A    , Local_A_stl);

    _cost = 2.0*size*size*size/3.0 + static_cast<double>(size*size);
  }


  // Invalidate copy constructor
  Action_parallel_lu_decomp(const Action_parallel_lu_decomp&)
  {
    INFOS("illegal call to Action_parallel_lu_decomp copy constructor");
    exit(1);
  }

  // Destructor
  ~Action_parallel_lu_decomp()
  {
    MESSAGE("Action_parallel_lu_decomp destructor");

    // Deallocation
    Interface::free_matrix(Local_A_ref, Local_A_stl.size());
    Interface::free_matrix(Local_A    , Local_A_stl.size());
  }

  // Action name
  static inline std::string name()
  {
    return "lu_decomp_" + Interface::name();
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
    Interface::parallel_lu_decomp(Local_A, desc);
  }

  BTL_DONT_INLINE void check_result()
  {
//    /* Gather */
//    typename Interface::stl_matrix Global_Y;
//    typename Interface::stl_matrix Local_Y(Local_A_stl.size());
//    Interface::matrix_to_stl(Local_A, Local_Y);
//    Interface::gather_matrix(Global_Y, Local_Y, desc);
//
//    if (iamroot) {
//
//      typename Interface::gene_matrix A;
//      Interface::matrix_from_stl(A, Global_A_stl);
//      lapack_interface<typename Interface::real_type>::lu_decomp(&Global_A_stl[0], A, _size);
//      typename Interface::stl_vector correct(A, A+_size*_size);
//      typename Interface::real_type error = STL_interface<typename Interface::real_type>::norm_diff(Global_Y, correct);
//      if (error > 10e-5)
//        cerr << " { !! error : " << error << " !! } ";
//
//      Interface::free_matrix(A, _size*_size);
//    }
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


#endif /* ACTION_PARALLEL_LU_DECOMP_HH_ */
