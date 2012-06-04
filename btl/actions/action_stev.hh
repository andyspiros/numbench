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
#ifndef ACTION_STEV
#define ACTION_STEV
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

using namespace std;

template<class Interface>
class Action_stev {

public :

  // Ctor

  Action_stev( int size ) : _size(size)
  {
    MESSAGE("Action_stev Ctor");

    // STL vector initialization
    init_vector<pseudo_random>(D_stl, _size);
	init_vector<pseudo_random>(E_stl, max(0, _size-1));
    init_matrix<null_function>(V_stl, _size);
	init_vector<null_function>(W_stl, _size);

    // generic matrix and vector initialization
    Interface::vector_from_stl(D_ref, D_stl);
	Interface::vector_from_stl(E_ref, E_stl);
    Interface::vector_from_stl(D, D_stl);
	Interface::vector_from_stl(E, E_stl);
    Interface::matrix_from_stl(V, V_stl);
    Interface::vector_from_stl(W, W_stl);

	// FIXME: correct?
    _cost = size*size*size;
  }

  // invalidate copy ctor

  Action_stev( const  Action_stev& )
  {
    INFOS("illegal call to Action_stev Copy Ctor");
    exit(1);
  }

  // Dtor

  ~Action_stev( void ){

    MESSAGE("Action_stev Dtor");

    // deallocation
    Interface::free_vector(D_ref);
	Interface::free_vector(E_ref);
    Interface::free_vector(D);
	Interface::free_vector(E);
    Interface::free_matrix(V, _size);
    Interface::free_vector(W);
  }

  // action name

  static inline std::string name()
  {
    return "stev_"+Interface::name();
  }

  double nb_op_base( void ){
    return _cost;
  }

  inline void initialize( void ){
    Interface::copy_vector(D_ref, D, _size);
    Interface::copy_vector(E_ref, E, _size-1);
  }

  inline void calculate( void ) {
      Interface::stev(D, E, W, V, _size);
  }

  void check_result( void ){
  }

private :

  typename Interface::stl_vector D_stl, E_stl;
  typename Interface::stl_matrix V_stl;
  typename Interface::stl_vector W_stl;

  typename Interface::gene_vector D_ref, E_ref;
  typename Interface::gene_vector D, E;
  typename Interface::gene_matrix V;
  typename Interface::gene_vector W;

  int _size;
  double _cost;
};

#endif
