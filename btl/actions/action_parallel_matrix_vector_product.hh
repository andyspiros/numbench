//=====================================================
// File   :  action_matrix_vector_product.hh
// Author :  L. Plagne <laurent.plagne@edf.fr)>
// Copyright (C) EDF R&D,  lun sep 30 14:23:19 CEST 2002
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
#ifndef ACTION_MATRIX_VECTOR_PRODUCT
#define ACTION_MATRIX_VECTOR_PRODUCT
#include "utilities.h"
#include "STL_interface.hh"
#include <string>
#include "init/init_function.hh"
#include "init/init_vector.hh"
#include "init/init_matrix.hh"

using namespace std;

template<class Interface>
class Action_parallel_matrix_vector_product {

public :

  // Ctor

  BTL_DONT_INLINE Action_parallel_matrix_vector_product( int size ):_size(size)
  {
    MESSAGE("Action_parallel_matrix_vector_product Ctor");
    const int iZERO = 0, iONE = 1;

    GlobalRows = _size;
    GlobalCols = _size;
    BlockRows = 2;
    BlockCols= 2;
    LocalXCols = 1;
    LocalYCols = 1;

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
      init_vector<pseudo_random>(Global_A_stl, GlobalRows*GlobalCols);
      init_vector<pseudo_random>(Global_x_stl, GlobalCols);
      init_vector<null_function>(Global_y_stl, GlobalRows);

      // Compute YTest (?)
    }

    Interface::scatter_matrix(Global_A_stl, Local_A_stl, GlobalRows, GlobalCols, BlockRows, BlockCols,  LocalRows, LocalCols);
    Interface::scatter_matrix(Global_x_stl, Local_x_stl, GlobalCols, LocalXCols, BlockRows, BlockCols, LocalXRows, LocalXCols);
    Interface::scatter_matrix(Global_y_stl, Local_y_stl, GlobalRows, LocalYCols, BlockRows, BlockCols, LocalYRows, LocalYCols);

    // generic local matrix and vectors initialization

    Interface::matrix_from_stl(Local_A_ref, Local_A_stl);
    Interface::matrix_from_stl(Local_A    , Local_A_stl);
    Interface::vector_from_stl(Local_x_ref, Local_x_stl);
    Interface::vector_from_stl(Local_x    , Local_x_stl);
    Interface::vector_from_stl(Local_y_ref, Local_y_stl);
    Interface::vector_from_stl(Local_y    , Local_y_stl);

    // Descinit
    int context = Interface::context();
    int info;
    descinit_(descA, &GlobalRows, &GlobalCols, &BlockRows, &BlockCols, &iZERO, &iZERO, &context,  &LocalRows, &info);
    descinit_(descX, &GlobalCols,       &iONE, &BlockRows, &BlockCols, &iZERO, &iZERO, &context, &LocalXRows, &info);
    descinit_(descY, &GlobalRows,       &iONE, &BlockRows, &BlockCols, &iZERO, &iZERO, &context, &LocalYRows, &info);
  }

  // invalidate copy ctor

  Action_parallel_matrix_vector_product( const  Action_parallel_matrix_vector_product & )
  {
    INFOS("illegal call to Action_parallel_matrix_vector_product Copy Ctor");
    exit(1);
  }

  // Dtor

  BTL_DONT_INLINE ~Action_parallel_matrix_vector_product( void ){

    MESSAGE("Action_parallel_matrix_vector_product Dtor");

    // deallocation

    Interface::free_matrix(Local_A_ref, GlobalRows*GlobalCols);;
    Interface::free_vector(Local_x_ref);
    Interface::free_vector(Local_y_ref);

    Interface::free_matrix(Local_A, GlobalRows*GlobalCols);;
    Interface::free_vector(Local_x);
    Interface::free_vector(Local_y);

  }

  // action name

  static inline std::string name( void )
  {
    return "parallel_matrix_vector_" + Interface::name();
  }

  double nb_op_base( void ){
    return 2.0*_size*_size;
  }

  BTL_DONT_INLINE  void initialize( void ){

    Interface::copy_matrix(Local_A_ref,Local_A,LocalRows*LocalCols);
    Interface::copy_vector(Local_x_ref,Local_x,LocalXRows*LocalXCols);
    Interface::copy_vector(Local_y_ref,Local_y,LocalYRows*LocalYCols);

  }

  BTL_DONT_INLINE void calculate( void ) {
      BTL_ASM_COMMENT("#begin matrix_vector_product");
      Interface::parallel_matrix_vector_product(GlobalRows, GlobalCols, Local_A, descA, Local_x, descX, Local_y, descY);
      BTL_ASM_COMMENT("end matrix_vector_product");
  }

  BTL_DONT_INLINE void check_result( void ){

    // calculation check

    // TODO

  }

private :

  typename Interface::stl_matrix Global_A_stl;
  typename Interface::stl_vector Global_x_stl;
  typename Interface::stl_vector Global_y_stl;

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
  int _size, GlobalRows, GlobalCols, LocalRows, LocalCols, BlockRows, BlockCols;
  int LocalXRows, LocalXCols, LocalYRows, LocalYCols;
  int descA[9], descX[9], descY[9];

};


#endif



