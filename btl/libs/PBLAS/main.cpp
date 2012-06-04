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
#define DISTRIBUTED

#include "mpi.h"
#include "utilities.h"
#include "bench.hh"

#include <iostream>
//using namespace std;

#include "pblas_interface.hh"
#include "blacsinit.hh"

#include "action_parallel_axpy.hh"
#include "action_parallel_matrix_vector_product.hh"
#include "action_parallel_lu_decomp.hh"
#include "action_parallel_cholesky.hh"
#include "action_parallel_qr_decomp.hh"
#include "action_parallel_svd_decomp.hh"
#include "action_parallel_symm_ev.hh"

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
  bool iamroot = blacsinit(&argc, &argv);

  bool
  axpy=false, matrix_vector=false,
  lu_decomp=false, cholesky=false, qr_decomp=false, svd_decomp=false,
  symm_ev=false
  ;

  int N = 100;


  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "axpy") axpy = true;
    else if (arg == "matrix_vector") matrix_vector=true;
    else if (arg == "lu_decomp") lu_decomp = true;
    else if (arg == "cholesky") cholesky = true;
    else if (arg == "qr_decomp") qr_decomp = true;
    else if (arg == "svd_decomp") svd_decomp = true;
    else if (arg == "symm_ev") symm_ev = true;

    // Check switch -N
    else if (arg[0] == '-' && arg[1] == 'N') {
        if (arg[2] != '\0')
            N = atoi(arg.c_str()+2);
        else
            N = atoi(argv[++i]);
    }
  }



  if (axpy)
  distr_bench<Action_parallel_axpy<pblas_interface<REAL_TYPE> > >(MIN_AXPY,MAX_AXPY, N, !iamroot);

  if (matrix_vector)
  distr_bench<Action_parallel_matrix_vector_product<pblas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV, N, !iamroot);

  if (lu_decomp)
  distr_bench<Action_parallel_lu_decomp<pblas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N, !iamroot);

  if (cholesky)
  distr_bench<Action_parallel_cholesky<pblas_interface<REAL_TYPE> > >(MIN_MM, MAX_MM,  N, !iamroot);

  if (qr_decomp)
  distr_bench<Action_parallel_qr_decomp<pblas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N, !iamroot);

  if (svd_decomp)
  distr_bench<Action_parallel_svd_decomp<pblas_interface<REAL_TYPE> > >(MIN_MM,750, N, !iamroot);

  if (symm_ev)
  distr_bench<Action_parallel_symm_ev<pblas_interface<REAL_TYPE> > >(MIN_MM,750, N, !iamroot);


  int iZERO = 0;
  blacs_exit_(&iZERO);
  return 0;
}


