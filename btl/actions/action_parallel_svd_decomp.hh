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
#ifndef ACTION_PARALLEL_SVD_DECOMP_HH_
#define ACTION_PARALLEL_SVD_DECOMP_HH_

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

#include "lapack_interface.hh"
#include "STL_interface.hh"

#include <string>
#include <algorithm>

template<class Interface>
class Action_parallel_svd_decomp {

public :

  // Constructor
  BTL_DONT_INLINE Action_parallel_svd_decomp( int size ) : _size(size)
  {
    MESSAGE("Action_parallel_svd_decomp Ctor");

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
      init_vector<pseudo_random>(Global_A_stl, size*size);
      init_vector<pseudo_random>(Global_U_stl, size*size);
      init_vector<pseudo_random>(Global_V_stl, size*size);
    }
    init_vector<null_function>(Local_s_stl, size);

    const int blocksize = std::max(std::min(size/4, 64), 2);
    Interface::scatter_matrix(Global_A_stl, Local_A_stl, descA, size, size, blocksize, blocksize);
    Interface::scatter_matrix(Global_U_stl, Local_U_stl, descU, size, size, blocksize, blocksize);
    Interface::scatter_matrix(Global_V_stl, Local_V_stl, descV, size, size, blocksize, blocksize);
    LocalRows = descA[8];
    LocalCols = Local_A_stl.size()/descA[8];

    // Generic local matrix and vectors initialization
    Interface::matrix_from_stl(Local_A_ref, Local_A_stl);
    Interface::matrix_from_stl(Local_A    , Local_A_stl);
    Interface::matrix_from_stl(Local_U_ref, Local_U_stl);
    Interface::matrix_from_stl(Local_U    , Local_U_stl);
    Interface::matrix_from_stl(Local_V_ref, Local_V_stl);
    Interface::matrix_from_stl(Local_V    , Local_V_stl);
    Interface::vector_from_stl(Local_s_ref, Local_s_stl);
    Interface::vector_from_stl(Local_s    , Local_s_stl);

    _cost = 2.0*size*size*size;
  }


  // Invalidate copy constructor
  Action_parallel_svd_decomp(const Action_parallel_svd_decomp&)
  {
    INFOS("illegal call to Action_parallel_svd_decomp copy constructor");
    exit(1);
  }

  // Destructor
  ~Action_parallel_svd_decomp()
  {
    MESSAGE("Action_parallel_svd_decomp destructor");

    // Deallocation
    Interface::free_matrix(Local_A_ref, Local_A_stl.size());
    Interface::free_matrix(Local_A    , Local_A_stl.size());
    Interface::free_matrix(Local_U_ref, Local_U_stl.size());
    Interface::free_matrix(Local_U    , Local_U_stl.size());
    Interface::free_matrix(Local_V_ref, Local_V_stl.size());
    Interface::free_matrix(Local_V    , Local_V_stl.size());
    Interface::free_vector(Local_s_ref);
    Interface::free_vector(Local_s    );
  }

  // Action name
  static inline std::string name()
  {
    return "svd_decomp_" + Interface::name();
  }

  double nb_op_base()
  {
    return _cost;
  }

  BTL_DONT_INLINE void initialize()
  {
    Interface::copy_matrix(Local_A_ref, Local_A, Local_A_stl.size());
    Interface::copy_matrix(Local_U_ref, Local_U, Local_U_stl.size());
    Interface::copy_matrix(Local_V_ref, Local_V, Local_V_stl.size());
    Interface::copy_vector(Local_s_ref, Local_s, Local_s_stl.size());
  }

  BTL_DONT_INLINE void calculate()
  {
    Interface::parallel_svd_decomp(Local_A, descA, Local_U, descU, Local_V, descV, Local_s);
  }

  BTL_DONT_INLINE void check_result()
  {
  }

private:
  int _size, descA[9], descU[9], descV[9], LocalRows, LocalCols;
  double _cost;
  bool iamroot;

  typename Interface::stl_matrix Global_A_stl;
  typename Interface::stl_matrix Local_A_stl;
  typename Interface::gene_matrix Local_A_ref;
  typename Interface::gene_matrix Local_A;

  typename Interface::stl_matrix Global_U_stl;
  typename Interface::stl_matrix Local_U_stl;
  typename Interface::gene_matrix Local_U_ref;
  typename Interface::gene_matrix Local_U;

  typename Interface::stl_matrix Global_V_stl;
  typename Interface::stl_matrix Local_V_stl;
  typename Interface::gene_matrix Local_V_ref;
  typename Interface::gene_matrix Local_V;

  typename Interface::stl_vector Local_s_stl;
  typename Interface::gene_vector Local_s_ref;
  typename Interface::gene_vector Local_s;
};


#endif /* ACTION_PARALLEL_SVD_DECOMP_HH */
