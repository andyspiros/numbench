//=====================================================
// Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
//=====================================================
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#ifndef ACTION_SYMM_EV
#define ACTION_SYMM_EV

#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

template<class Interface>
class Action_symm_ev {

public:

	// Ctor
	Action_symm_ev( int size ) : _size(size)
	{
	    MESSAGE("Action_symm_ev Ctor");

	    // STL matrix initialization
	    init_matrix_symm<pseudo_random>(X_stl,_size);
	    init_vector<pseudo_random>(W_stl,_size);

	    // generic matrix and vector initialization
	    Interface::matrix_from_stl(X,X_stl);
	    Interface::vector_from_stl(W,W_stl);


	}

	  // invalidate copy ctor

	  Action_symm_ev( const  Action_symm_ev & )
	  {
	    INFOS("illegal call to Action_symm_ev Copy Ctor");
	    exit(1);
	  }

	  // Dtor

	  ~Action_symm_ev( void ){

	    MESSAGE("Action_symm_ev Dtor");

	    // deallocation
	    Interface::free_matrix(X,_size);
	  }

	  // action name

	  static inline std::string name( void )
	  {
	    return "symm_ev_"+Interface::name();
	  }

	  double nb_op_base( void ){
		// TODO: is this right?
	    return _size*_size;
	  }

	  inline void initialize( void ){
	  }

	  inline void calculate( void ) {
	      Interface::symm_ev(X,W,_size);
	  }

	  void check_result( void ) {
		  // TODO: check result
	  }

private :

  typename Interface::stl_matrix X_stl;
  typename Interface::stl_vector W_stl;

  typename Interface::gene_matrix X;
  typename Interface::gene_vector W;

  int _size;
  double _cost;

};


#endif // ACTION_SYMM_EV
