//=====================================================
// File   :  main.cpp
// Author :  L. Plagne <laurent.plagne@edf.fr)>
// Copyright (C) EDF R&D,  lun sep 30 14:23:28 CEST 2002
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

#ifndef BLAS_INTERFACE
#  ifndef CBLAS_INTERFACE
#    define BLAS_INTERFACE
#  endif
#endif

#include "utilities.h"
#include "blas_interface.hh"
#include "bench.hh"
#include "basic_actions.hh"
#include "action_trisolve_matrix.hh"

#include <string>

BTL_MAIN;

int main(int argv, char **argc)
{
  bool
  axpy=false, axpby=false, rot=false,
  matrix_vector=false, atv=false, symv=false, syr2=false, ger=false, trisolve_vector=false,
  matrix_matrix=false, aat=false, trisolve_matrix=false, trmm=false  
  ;
  int N = 100;
  
  
  for (int i = 1; i < argv; ++i) {
    std::string arg = argc[i];
    if (arg == "axpy") axpy = true;
    else if (arg == "axpby") axpby = true;
    else if (arg == "rot") rot = true;
    else if (arg == "matrix_vector") matrix_vector = true;
    else if (arg == "atv") atv = true;
    else if (arg == "symv") symv = true;
    else if (arg == "syr2") syr2 = true;
    else if (arg == "ger") ger = true;
    else if (arg == "trisolve_vector") trisolve_vector = true;
    else if (arg == "matrix_matrix") matrix_matrix = true;
    else if (arg == "aat") aat = true;
    else if (arg == "trisolve_matrix") trisolve_matrix = true;
    else if (arg == "trmm") trmm = true;
    
    else if (arg[0] == '1' && arg[1] == '\0') {
      axpy = true; axpby = true; rot = true;
    }
    else if (arg[0] == '2' && arg[1] == '\0') {
      matrix_vector=true; atv=true; symv=true; syr2=true; ger=true; trisolve_vector=true;
    }
    else if (arg[0] == '3' && arg[1] == '\0') {
      matrix_matrix=true; aat=true; trisolve_matrix=true; trmm=true;
    }

    // Check switch -N
    else if (arg[0] == '-' && arg[1] == 'N') {
        if (arg[2] != '\0')
            N = atoi(arg.c_str()+2);
        else
            N = atoi(argc[++i]);
    }
  }

  if (axpy)
  bench<Action_axpy<blas_interface<REAL_TYPE> > >(MIN_AXPY,MAX_AXPY, N);
  if (axpby)
  bench<Action_axpby<blas_interface<REAL_TYPE> > >(MIN_AXPY,MAX_AXPY, N);
  if (rot)
  bench<Action_rot<blas_interface<REAL_TYPE> > >(MIN_AXPY,MAX_AXPY, N);

  if (matrix_vector)
  bench<Action_matrix_vector_product<blas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV, N);
  if (atv)
  bench<Action_atv_product<blas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV, N);
  if (symv)
  bench<Action_symv<blas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV, N);
  if (syr2)
  bench<Action_syr2<blas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV, N);
  if (ger)
  bench<Action_ger<blas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV, N);
  if (trisolve_vector)
  bench<Action_trisolve<blas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N);

  if (matrix_matrix)
  bench<Action_matrix_matrix_product<blas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N);
  if (aat)
  bench<Action_aat_product<blas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N);
  if (trisolve_matrix)
  bench<Action_trisolve_matrix<blas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N);
  if (trmm)
  bench<Action_trmm<blas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM, N);


  return 0;
}


