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
#ifndef ACTION_SVD_DECOMP
#define ACTION_SVD_DECOMP
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

using namespace std;

template<class Interface>
class Action_svd_decomp {

public :

  // Ctor

  Action_svd_decomp( int size ) : _size(size)
  {
    MESSAGE("Action_svd_decomp Ctor");

    // STL vector initialization
    init_matrix<pseudo_random>(X_stl ,_size);
    init_matrix<null_function>(U_stl, _size);
    init_matrix<null_function>(VT_stl, _size);
	init_vector<null_function>(S_stl, _size);

    // generic matrix and vector initialization
    Interface::matrix_from_stl(X_ref, X_stl);
    Interface::matrix_from_stl(X, X_stl);
    Interface::matrix_from_stl(U, U_stl);
    Interface::matrix_from_stl(VT, VT_stl);
    Interface::vector_from_stl(S, S_stl);

    _cost = 4*size*size*size;
  }

  // invalidate copy ctor

  Action_svd_decomp( const  Action_svd_decomp & )
  {
    INFOS("illegal call to Action_svd_decomp Copy Ctor");
    exit(1);
  }

  // Dtor

  ~Action_svd_decomp( void ){

    MESSAGE("Action_svd_decomp Dtor");

    // deallocation
    Interface::free_matrix(X_ref, _size);
    Interface::free_matrix(X, _size);
    Interface::free_matrix(U, _size);
    Interface::free_matrix(VT, _size);
    Interface::free_vector(S);
  }

  // action name

  static inline std::string name()
  {
    return "svd_decomp_"+Interface::name();
  }

  double nb_op_base( void ){
    return _cost;
  }

  inline void initialize( void ){
    Interface::copy_matrix(X_ref, X, _size);
  }

  inline void calculate( void ) {
      Interface::svd_decomp(X, U, S, VT, _size);
  }

  void check_result( void ){
  }

private :

  typename Interface::stl_matrix X_stl;
  typename Interface::stl_matrix U_stl, VT_stl;
  typename Interface::stl_vector S_stl;

  typename Interface::gene_matrix X_ref;
  typename Interface::gene_matrix X;
  typename Interface::gene_matrix U, VT;
  typename Interface::gene_vector S;

  int _size;
  double _cost;
};

#endif
