#ifndef ACTION_GENERAL_SOLVE
#define ACTION_GENERAL_SOLVE

#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

using namespace std;

template<class Interface>
class Action_general_solve {

public:

	// Ctor
	Action_general_solve( int size ) : _size(size)
	{
	    MESSAGE("Action_general_solve Ctor");

	    // STL matrix and vector initialization

	    init_matrix<pseudo_random>(A_stl,_size);
	    init_vector<pseudo_random>(B_stl,_size);
	    init_vector<null_function>(X_stl,_size);
	    init_vector<null_function>(resu_stl,_size);

	    // generic matrix and vector initialization

	    Interface::matrix_from_stl(A_ref,A_stl);
	    Interface::matrix_from_stl(A,A_stl);
	    Interface::vector_from_stl(B_ref,B_stl);
	    Interface::vector_from_stl(B,B_stl);
	    Interface::vector_from_stl(X_ref,X_stl);
	    Interface::vector_from_stl(X,X_stl);


	}

	// invalidate copy ctor

	Action_general_solve( const  Action_general_solve & )
	{
	    INFOS("illegal call to Action_general_solve Copy Ctor");
	    exit(0);
	}

	// Dtor

	BTL_DONT_INLINE ~Action_general_solve( void ){

	    MESSAGE("Action_general_solve Dtor");

	    // deallocation

	    Interface::free_matrix(A,_size);
	    Interface::free_vector(B);
	    Interface::free_vector(X);

	    Interface::free_matrix(A_ref,_size);
	    Interface::free_vector(B_ref);
	    Interface::free_vector(X_ref);

	}

	  // action name

	  static inline std::string name( void )
	  {
	    return "general_solve_" + Interface::name();
	  }

	  double nb_op_base( void ){
	    return 2.0*_size*_size*_size;
	  }

	  BTL_DONT_INLINE  void initialize( void ){

	    Interface::copy_matrix(A_ref,A,_size);
	    Interface::copy_vector(B_ref,B,_size);
	    Interface::copy_vector(X_ref,X,_size);

	  }

	  BTL_DONT_INLINE void calculate( void ) {
	      BTL_ASM_COMMENT("#begin general_solve");
	      Interface::general_solve(A,B,X,_size);
	      BTL_ASM_COMMENT("end general_solve");
	  }

	  BTL_DONT_INLINE void check_result() {
		  // TODO: check result
	  }


private:

	  typename Interface::stl_matrix A_stl;
	  typename Interface::stl_vector B_stl;
	  typename Interface::stl_vector X_stl;
	  typename Interface::stl_vector resu_stl;

	  typename Interface::gene_matrix A_ref;
	  typename Interface::gene_vector B_ref;
	  typename Interface::gene_vector X_ref;

	  typename Interface::gene_matrix A;
	  typename Interface::gene_vector B;
	  typename Interface::gene_vector X;


	  int _size;


};


#endif// ACTION_GENERAL_SOLVE
