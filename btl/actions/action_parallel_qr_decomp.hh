#ifndef ACTION_PARALLEL_QR_DECOMP_HH_
#define ACTION_PARALLEL_QR_DECOMP_HH_

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

#include "lapack_interface.hh"
#include "STL_interface.hh"

#include <string>
#include <algorithm>

template<class Interface>
class Action_parallel_qr_decomp {

public :

  // Constructor
  BTL_DONT_INLINE Action_parallel_qr_decomp( int size ) : _size(size)
  {
    MESSAGE("Action_parallel_qr_decomp Ctor");

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
        /* Using a constant seed */
        const unsigned seed = 3;
        init_matrix(Global_A_stl, size, seed);
    }

    const int blocksize = std::max(std::min(size/4, 64), 2);
    scatter_matrix(Global_A_stl, Local_A_stl, desc, size, size, blocksize, blocksize);
    LocalRows = desc[8];
    LocalCols = Local_A_stl.size()/desc[8];

    // Generic local matrix and vectors initialization
    Interface::matrix_from_stl(Local_A    , Local_A_stl);

    _cost = 2.0*size*size*size;
  }


  // Invalidate copy constructor
  Action_parallel_qr_decomp(const Action_parallel_qr_decomp&)
  {
    INFOS("illegal call to Action_parallel_qr_decomp copy constructor");
    exit(1);
  }

  // Destructor
  ~Action_parallel_qr_decomp()
  {
    MESSAGE("Action_parallel_qr_decomp destructor");

    // Deallocation
    Interface::free_matrix(Local_A    , Local_A_stl.size());
  }

  // Action name
  static inline std::string name()
  {
    return "qr_decomp_" + Interface::name();
  }

  double nb_op_base()
  {
    return _cost;
  }

  BTL_DONT_INLINE void initialize()
  {
  }

  BTL_DONT_INLINE void calculate()
  {
    Interface::copy_matrix(&Local_A_stl[0], Local_A, Local_A_stl.size());
    Interface::parallel_qr_decomp(Local_A, desc);
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
  typename Interface::gene_matrix Local_A;
};


#endif /* ACTION_PARALLEL_QR_DECOMP_HH_ */
