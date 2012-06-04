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
#ifndef ACTION_PARALLEL_SYMM_EV_HH_
#define ACTION_PARALLEL_SYMM_EV_HH_

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

#include "lapack_interface.hh"
#include "STL_interface.hh"

#include <string>

template<class Interface>
class Action_parallel_symm_ev {

public :

  // Constructor
  BTL_DONT_INLINE Action_parallel_symm_ev( int size ) : _size(size)
  {
    MESSAGE("Action_parallel_symm_ev constructor");

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
      init_vector<pseudo_random>(Global_A_stl, size*size);
      init_vector<null_function>(Global_Z_stl, size*size);
    }
    init_vector<null_function>(Local_w_stl, size);

    const int blocksize = std::max(std::min(size/4, 64), 2);
    Interface::scatter_matrix(Global_A_stl, Local_A_stl, descA, size, size, blocksize, blocksize);
    Interface::scatter_matrix(Global_Z_stl, Local_Z_stl, descZ, size, size, blocksize, blocksize);
    LocalRows = descA[8];
    LocalCols = Local_A_stl.size()/descA[8];

    // Generic local matrix and vectors initialization
    Interface::matrix_from_stl(Local_A_ref, Local_A_stl);
    Interface::matrix_from_stl(Local_A    , Local_A_stl);
    Interface::matrix_from_stl(Local_Z_ref, Local_Z_stl);
    Interface::matrix_from_stl(Local_Z    , Local_Z_stl);
    Interface::vector_from_stl(Local_w    , Local_w_stl);
    Interface::vector_from_stl(Local_w_ref, Local_w_stl);

    _cost = size*size*size;
  }


  // Invalidate copy constructor
  Action_parallel_symm_ev(const Action_parallel_symm_ev&)
  {
    INFOS("illegal call to Action_parallel_symm_ev copy constructor");
    exit(1);
  }

  // Destructor
  ~Action_parallel_symm_ev()
  {
    MESSAGE("Action_parallel_symm_ev destructor");

    // Deallocation
    Interface::free_matrix(Local_A_ref, Local_A_stl.size());
    Interface::free_matrix(Local_A    , Local_A_stl.size());
    Interface::free_matrix(Local_Z_ref, Local_Z_stl.size());
    Interface::free_matrix(Local_Z    , Local_Z_stl.size());
    Interface::free_vector(Local_w_ref);
    Interface::free_vector(Local_w    );
  }

  // Action name
  static inline std::string name()
  {
    return "symm_ev_" + Interface::name();
  }

  double nb_op_base()
  {
    return _cost;
  }

  BTL_DONT_INLINE void initialize()
  {
    Interface::copy_matrix(Local_A_ref, Local_A, Local_A_stl.size());
    Interface::copy_matrix(Local_Z_ref, Local_Z, Local_Z_stl.size());
    Interface::copy_vector(Local_w_ref, Local_w, Local_w_stl.size());
  }

  BTL_DONT_INLINE void calculate()
  {
    Interface::parallel_symm_ev(Local_A, descA, Local_w, Local_Z, descZ);
  }

  BTL_DONT_INLINE void check_result()
  {
  }

private:
  int _size, descA[9], descZ[9], LocalRows, LocalCols;
  double _cost;
  bool iamroot;

  typename Interface::stl_matrix Global_A_stl;
  typename Interface::stl_matrix Local_A_stl;
  typename Interface::gene_matrix Local_A_ref;
  typename Interface::gene_matrix Local_A;

  typename Interface::stl_matrix Global_Z_stl;
  typename Interface::stl_matrix Local_Z_stl;
  typename Interface::gene_matrix Local_Z_ref;
  typename Interface::gene_matrix Local_Z;

  typename Interface::stl_vector Local_w_stl;
  typename Interface::gene_vector Local_w_ref;
  typename Interface::gene_vector Local_w;
};


#endif /* ACTION_PARALLEL_SYMM_EV_HH_ */
