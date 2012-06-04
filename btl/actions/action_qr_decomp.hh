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
#ifndef ACTION_QR_DECOMP
#define ACTION_QR_DECOMP
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

using namespace std;

template<class Interface>
class Action_qr_decomp {

public :

  // Ctor

  Action_qr_decomp( int size ) : _size(size)
  {
    MESSAGE("Action_qr_decomp Ctor");

    // STL vector initialization
    init_matrix<pseudo_random>(X_stl ,_size);
    init_matrix<null_function>(C_stl, _size);
	init_vector<null_function>(tau_stl, _size);

    // generic matrix and vector initialization
    Interface::matrix_from_stl(X_ref, X_stl);
    Interface::matrix_from_stl(X, X_stl);
    Interface::matrix_from_stl(C, C_stl);
    Interface::vector_from_stl(tau, tau_stl);
	
    _cost = 2.0*size*size*size/3.0 + size*size + 1.0*size/3.0 - 2;
  }

  // invalidate copy ctor

  Action_qr_decomp( const  Action_qr_decomp & )
  {
    INFOS("illegal call to Action_qr_decomp Copy Ctor");
    exit(1);
  }

  // Dtor

  ~Action_qr_decomp( void ){

    MESSAGE("Action_qr_decomp Dtor");

    // deallocation
    Interface::free_matrix(X_ref, _size);
    Interface::free_matrix(X, _size);
    Interface::free_matrix(C, _size);
    Interface::free_vector(tau);
  }

  // action name

  static inline std::string name()
  {
    return "qr_decomp_"+Interface::name();
  }

  double nb_op_base( void ){
    return _cost;
  }

  inline void initialize( void ){
    Interface::copy_matrix(X_ref, X, _size);
  }

  inline void calculate( void ) {
      Interface::qr_decomp(X, C, tau, _size);
  }

  void check_result( void ){
  }

private :

  typename Interface::stl_matrix X_stl;
  typename Interface::stl_matrix C_stl;
  typename Interface::stl_vector tau_stl;

  typename Interface::gene_matrix X_ref;
  typename Interface::gene_matrix X;
  typename Interface::gene_matrix C;
  typename Interface::gene_vector tau;

  int _size;
  double _cost;
};

#endif
