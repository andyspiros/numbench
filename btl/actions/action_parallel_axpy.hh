#ifndef ACTION_PARALLEL_AXPY
#define ACTION_PARALLEL_AXPY
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include <algorithm>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

#include "blas.h"

using namespace std;

template<class Interface>
class Action_parallel_axpy {
public:
  BTL_DONT_INLINE Action_parallel_axpy( int size ):_size(size), _coef(1.)
  {
    MESSAGE("Action_parallel_axpy Ctor");
    int iZERO = 0, iONE = 1;

    GlobalRows = _size;
    GlobalCols = 1;
    BlockRows = 2;
    BlockCols= 1;

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
      init_vector<pseudo_random>(Global_x_stl, _size);
      init_vector<pseudo_random>(Global_y_stl, _size);
    }

    Interface::scatter_matrix(Global_x_stl, Local_x_stl, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);
    Interface::scatter_matrix(Global_y_stl, Local_y_stl, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);

    Interface::vector_from_stl(Local_x_ref, Local_x_stl);
    Interface::vector_from_stl(Local_x    , Local_x_stl);
    Interface::vector_from_stl(Local_y_ref, Local_y_stl);
    Interface::vector_from_stl(Local_y    , Local_y_stl);

    // Descinit
    int context = Interface::context();
    int info;
    int LD = std::max(1, LocalRows);
    descinit_(descX, &_size, &iONE, &BlockRows, &BlockCols, &iZERO, &iZERO, &context, &LD, &info);
    descinit_(descY, &_size, &iONE, &BlockRows, &BlockCols, &iZERO, &iZERO, &context, &LD, &info);

    // Copy Y to Test_y
    Test_y_stl = Global_y_stl;
  }

  // Invalidate copy constructor
  Action_parallel_axpy (const Action_parallel_axpy&)
  {
    INFOS("illegal call to Action_parallel_axpy Copy Ctor");
    exit(1);
  }

  // Destructor
  BTL_DONT_INLINE ~Action_parallel_axpy(){

    MESSAGE("Action_parallel_axpy Dtor");

    // deallocation

    Interface::free_vector(Local_x_ref);
    Interface::free_vector(Local_y_ref);

    Interface::free_vector(Local_x);
    Interface::free_vector(Local_y);
  }

  // action name
  static inline std::string name()
  {
    return "axpy_" + Interface::name();
  }

  double nb_op_base( void ){
    return 2.0*_size;
  }

  BTL_DONT_INLINE  void initialize(){
    Interface::copy_vector(Local_x_ref, Local_x, LocalRows*LocalCols);
    Interface::copy_vector(Local_y_ref, Local_y, LocalRows*LocalCols);
  }

  BTL_DONT_INLINE void calculate( void ) {
      BTL_ASM_COMMENT("#begin axpy");
      Interface::parallel_axpy(_coef, Local_x, descX, Local_y, descY, _size);
      BTL_ASM_COMMENT("end axpy");
  }

  void check_result( void ){
    int iONE = 1;
    Interface::vector_to_stl(Local_y, Local_y_stl);
    Interface::gather_matrix(Global_y_stl, Local_y_stl, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);

    // calculation check
    if (iamroot) {

      // Compute YTest
      STL_interface<typename Interface::real_type>::axpy(_coef, Global_x_stl, Test_y_stl, _size);

      typename Interface::real_type error =
            STL_interface<typename Interface::real_type>::norm_diff(Global_y_stl, Test_y_stl);

      if (error > 1e-5)
        std::cerr << "Error: " << error << std::endl;
    }

  }

private:
  int _size;
  int GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols;
  typename Interface::real_type _coef;
  bool iamroot;

  typename Interface::stl_vector Global_x_stl;
  typename Interface::stl_vector Global_y_stl;
  typename Interface::stl_vector Test_y_stl;

  typename Interface::stl_vector Local_x_stl;
  typename Interface::stl_vector Local_y_stl;

  typename Interface::gene_vector Local_x_ref;
  typename Interface::gene_vector Local_y_ref;

  typename Interface::gene_vector Local_x;
  typename Interface::gene_vector Local_y;

  int descX[9], descY[9];
};

#endif
