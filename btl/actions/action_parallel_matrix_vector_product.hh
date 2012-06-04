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
#ifndef ACTION_PARALLEL_MATRIX_VECTOR_PRODUCT
#define ACTION_PARALLEL_MATRIX_VECTOR_PRODUCT
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include <algorithm>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

#include "blas.h"

template<class Interface>
class Action_parallel_matrix_vector_product {

public :

  // Ctor

  BTL_DONT_INLINE Action_parallel_matrix_vector_product( int size ):_size(size)
  {
    MESSAGE("Action_parallel_matrix_vector_product Ctor");
    int iZERO = 0, iONE = 1;

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
      init_vector<pseudo_random>(Global_A_stl, size*size);
      init_vector<pseudo_random>(Global_x_stl, size);
      init_vector<null_function>(Global_y_stl, size);
    }

    const int blocksize = std::max(std::min(size/4, 64), 2);
    Interface::scatter_matrix(Global_A_stl, Local_A_stl, descA, size, size, blocksize, blocksize);
    Interface::scatter_matrix(Global_x_stl, Local_x_stl, descX, size,    1, blocksize, blocksize);
    Interface::scatter_matrix(Global_y_stl, Local_y_stl, descY, size,    1, blocksize, blocksize);

    // generic local matrix and vectors initialization

    Interface::matrix_from_stl(Local_A_ref, Local_A_stl);
    Interface::matrix_from_stl(Local_A    , Local_A_stl);
    Interface::vector_from_stl(Local_x_ref, Local_x_stl);
    Interface::vector_from_stl(Local_x    , Local_x_stl);
    Interface::vector_from_stl(Local_y_ref, Local_y_stl);
    Interface::vector_from_stl(Local_y    , Local_y_stl);
  }

  // invalidate copy ctor
  Action_parallel_matrix_vector_product( const  Action_parallel_matrix_vector_product & )
  {
    INFOS("illegal call to Action_parallel_matrix_vector_product copy constructor");
    exit(1);
  }

  // Dtor
  BTL_DONT_INLINE ~Action_parallel_matrix_vector_product(){

    MESSAGE("Action_parallel_matrix_vector_product destructor");

    // deallocation

    Interface::free_matrix(Local_A_ref, _size*_size);
    Interface::free_vector(Local_x_ref);
    Interface::free_vector(Local_y_ref);

    Interface::free_matrix(Local_A, _size*_size);
    Interface::free_vector(Local_x);
    Interface::free_vector(Local_y);

  }

  // action name
  static inline std::string name( void )
  {
    return "matrix_vector_" + Interface::name();
  }

  double nb_op_base( void ){
    return 2.0*_size*_size;
  }

  BTL_DONT_INLINE  void initialize( void ){
    Interface::copy_matrix(Local_A_ref, Local_A, Local_A_stl.size());
    Interface::copy_vector(Local_x_ref, Local_x, Local_x_stl.size());
    Interface::copy_vector(Local_y_ref, Local_y, Local_y_stl.size());
  }

  BTL_DONT_INLINE void calculate( void ) {
      Interface::parallel_matrix_vector_product(_size, _size, Local_A, descA, Local_x, descX, Local_y, descY);
  }

  BTL_DONT_INLINE void check_result( void ){
    int GlobalYCols;
    Interface::vector_to_stl(Local_y, Local_y_stl);

    Interface::gather_matrix(Global_y_stl, Local_y_stl, descY);

    // calculation check
    if (iamroot) {

      // Compute YTest
      Test_y_stl.resize(_size);
      STL_interface<typename Interface::real_type>::matrix_vector_product(Global_A_stl, Global_x_stl, Test_y_stl, _size);

      typename Interface::real_type error = STL_interface<typename Interface::real_type>::norm_diff(Global_y_stl, Test_y_stl);

      if (error > 1e-5)
        std::cerr << "Error: " << error << " ";
    }

  }

private :

  typename Interface::stl_matrix Global_A_stl;
  typename Interface::stl_vector Global_x_stl;
  typename Interface::stl_vector Global_y_stl;
  typename Interface::stl_vector Test_y_stl;

  typename Interface::stl_matrix Local_A_stl;
  typename Interface::stl_vector Local_x_stl;
  typename Interface::stl_vector Local_y_stl;

  typename Interface::gene_matrix Local_A_ref;
  typename Interface::gene_vector Local_x_ref;
  typename Interface::gene_vector Local_y_ref;

  typename Interface::gene_matrix Local_A;
  typename Interface::gene_vector Local_x;
  typename Interface::gene_vector Local_y;

  bool iamroot;
  int _size;
  int descA[9], descX[9], descY[9];

};


#endif
