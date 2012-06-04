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
#ifndef ACTION_PARALLEL_LU_DECOMP_HH_
#define ACTION_PARALLEL_LU_DECOMP_HH_

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

#include "lapack_interface.hh"
#include "STL_interface.hh"

#include <string>

template<class Interface>
class Action_parallel_lu_decomp {

public :

  // Constructor
  BTL_DONT_INLINE Action_parallel_lu_decomp( int size ) : _size(size)
  {
    MESSAGE("Action_parallel_lu_decomp Ctor");

    int myid, procnum;
    blacs_pinfo_(&myid, &procnum);
    iamroot = (myid == 0);

    // STL matrix and vector initialization
    if (iamroot) {
        /* Using a constant seed */
        const unsigned seed = 3;
        init_matrix(Global_A_stl, size, seed);
    }

    const int blocksize = std::max(std::min(size/4, 64), 2);
    scatter_matrix(Global_A_stl, Local_A_stl, desc, size, size, blocksize, blocksize);
    LocalRows = desc[8];
    LocalCols = Local_A_stl.size()/desc[8];

    // Generic local matrix and vectors initialization
    Interface::matrix_from_stl(Local_A    , Local_A_stl);

    _cost = 2.0*size*size*size/3.0 + static_cast<double>(size*size);
  }


  // Invalidate copy constructor
  Action_parallel_lu_decomp(const Action_parallel_lu_decomp&)
  {
    INFOS("illegal call to Action_parallel_lu_decomp copy constructor");
    exit(1);
  }

  // Destructor
  ~Action_parallel_lu_decomp()
  {
    MESSAGE("Action_parallel_lu_decomp destructor");

    // Deallocation
    Interface::free_matrix(Local_A    , Local_A_stl.size());
  }

  // Action name
  static inline std::string name()
  {
    return "lu_decomp_" + Interface::name();
  }

  double nb_op_base()
  {
    return _cost;
  }

  BTL_DONT_INLINE void initialize()
  {
  }

  BTL_DONT_INLINE void calculate()
  {
    Interface::copy_matrix(&Local_A_stl[0], Local_A, Local_A_stl.size());
    Interface::parallel_lu_decomp(Local_A, ipiv_stl, desc);
  }

  BTL_DONT_INLINE void check_result()
  {
//    /* Gather */
//    typename Interface::stl_matrix Global_Y;
//    typename Interface::stl_matrix Local_Y(Local_A_stl.size());
//    Interface::matrix_to_stl(Local_A, Local_Y);
//    Interface::gather_matrix(Global_Y, Local_Y, desc);
//
//    if (iamroot) {
//
//      typename Interface::gene_matrix A;
//      Interface::matrix_from_stl(A, Global_A_stl);
//      lapack_interface<typename Interface::real_type>::lu_decomp(&Global_A_stl[0], A, _size);
//      typename Interface::stl_vector correct(A, A+_size*_size);
//      typename Interface::real_type error = STL_interface<typename Interface::real_type>::norm_diff(Global_Y, correct);
//      if (error > 10e-5)
//        cerr << " { !! error : " << error << " !! } ";
//
//      Interface::free_matrix(A, _size*_size);
//    }


//    if (_size > 2) {
//      double error = Interface::test_LU(Global_A_stl, Local_A, desc);
//      if (iamroot)
//        cout << " {error: " << error << "} ";
//    }
  }

private:
  int _size, desc[9], LocalRows, LocalCols;
  double _cost;
  bool iamroot;

  typename Interface::stl_matrix Global_A_stl;
  typename Interface::stl_matrix Local_A_stl;
  typename Interface::gene_matrix Local_A;

  std::vector<int> ipiv_stl;
};


#endif /* ACTION_PARALLEL_LU_DECOMP_HH_ */
