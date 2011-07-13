#ifndef ACTION_FFTW_1D_BACKWARD_ESTIMATE
#define ACTION_FFTW_1D_BACKWARD_ESTIMATE
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include <cmath>
#include "init/init_function.hh"
#include "init/init_vector.hh"

using namespace std;

template<class Interface>
class Action_FFTW_1D_Backward_Estimate {

public :

  // Ctor

	Action_FFTW_1D_Backward_Estimate( int size ):_size(size)
  {
    MESSAGE("Action_FFTW_1D_Backward_Estimate Ctor");

    // STL vector initialization

    init_vector<pseudo_random>(X_stl,_size);
    init_vector<null_function>(Y_stl,_size);

    // generic matrix and vector initialization

    Interface::vector_from_stl(X,X_stl);
    Interface::vector_from_stl(Y,Y_stl);

	  Interface::fftw_init_plan(p,_size, X, Y, FFTW_FORWARD, FFTW_MEASURE);

  }

  // invalidate copy ctor

	Action_FFTW_1D_Backward_Estimate( const  Action_FFTW_1D_Backward_Estimate & )
  {
    INFOS("illegal call to Action_FFTW_1D_Backward_Estimate Copy Ctor");
    exit(1);
  }

  // Dtor

  ~Action_FFTW_1D_Backward_Estimate( void ){

    MESSAGE("Action_FFTW_1D_Backward_Estimate Dtor");

    // deallocation

    Interface::free_vector(X);
    Interface::free_vector(Y);
  }

  // action name

  static inline std::string name( void )
  {
    return "FFTW_1D_Backward_Estimate_"+Interface::name();
  }

  double nb_op_base( void ){
	const static double log_2 = log(2.);
    return _size * log(_size)/log_2;
  }

  inline void initialize( void ){
  }

  inline void calculate( void ) {
    BTL_ASM_COMMENT("mybegin FFTW_1D_Backward_Estimate");
    Interface::fftw_run(p);
    BTL_ASM_COMMENT("myend FFTW_1D_Backward_Estimate");
  }

  void check_result( void ){
  }

private :

  typename Interface::stl_vector X_stl;
  typename Interface::stl_vector Y_stl;

  typename Interface::gene_vector X;
  typename Interface::gene_vector Y;

  typename Interface::plan p;

  int _size;
};

#endif // ACTION_FFTW_1D_BACKWARD_ESTIMATE
