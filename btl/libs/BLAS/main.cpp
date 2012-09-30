//=====================================================
// Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
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

// Include the numeric interface
#ifdef NI_BLAS
#  define NI_FORTRAN
#endif

#include "NumericInterface.hpp"

typedef NumericInterface<double> Interface;

// Include the BTL
#include "utilities.h"
#include "bench.hh"

// Include the operations
#include "actionsBLAS.hpp"

#include <string>
#include <iostream>

using namespace std;

BTL_MAIN;

int main(int argv, char **argc)
{
    bool
    do_axpy=false, do_rot=false,

    do_MatrixVector=false, do_MatrixTVector=false, do_SymMatrixVector=false,
    do_Rank1Update=false, do_Rank2Update=false, do_TriSolveVector=false,

    do_MatrixMatrix=false, do_MatrixTMatrix=false,
    do_TriMatrixMatrix=false, do_TriSolveMatrix=false
    ;

    int N = 100;


    for (int i = 1; i < argv; ++i) {
      std::string arg = argc[i];

           if (arg == "axpy") do_axpy = true;
      else if (arg == "rot") do_rot = true;

      else if (arg == "MatrixVector") do_MatrixVector = true;
      else if (arg == "MatrixTVector") do_MatrixTVector = true;
      else if (arg == "SymMatrixVector") do_SymMatrixVector = true;
      else if (arg == "Rank1Update") do_Rank1Update = true;
      else if (arg == "Rank2Update") do_Rank2Update = true;
      else if (arg == "TriSolveVector") do_TriSolveVector = true;

      else if (arg == "MatrixMatrix") do_MatrixMatrix = true;
      else if (arg == "MatrixTMatrix") do_MatrixTMatrix = true;
      else if (arg == "TriMatrixMatrix") do_TriMatrixMatrix = true;
      else if (arg == "TriSolveMatrix") do_TriSolveMatrix = true;

      // Check switch -N
      else if (arg[0] == '-' && arg[1] == 'N') {
          if (arg[2] != '\0')
              N = atoi(arg.c_str()+2);
          else
              N = atoi(argc[++i]);
      }
    }

    if (do_axpy)
    bench<Action_axpy<Interface> >(MIN_AXPY,MAX_AXPY, N);
    if (do_rot)
    bench<Action_rot<Interface> >(MIN_AXPY,MAX_AXPY, N);

    if (do_MatrixVector)
    bench<Action_MatrixVector<Interface> >(MIN_MV,MAX_MV, N);
    if (do_MatrixTVector)
    bench<Action_MatrixTVector<Interface> >(MIN_MV,MAX_MV, N);
    if (do_SymMatrixVector)
    bench<Action_SymMatrixVector<Interface> >(MIN_MV,MAX_MV, N);
    if (do_Rank1Update)
    bench<Action_Rank1Update<Interface> >(MIN_MV,MAX_MV, N);
    if (do_Rank2Update)
    bench<Action_Rank2Update<Interface> >(MIN_MV,MAX_MV, N);
    if (do_TriSolveVector)
    bench<Action_TriSolveVector<Interface> >(MIN_MM,MAX_MM, N);

    if (do_MatrixMatrix)
    bench<Action_MatrixMatrix<Interface> >(MIN_MM,MAX_MM, N);
    if (do_MatrixTMatrix)
    bench<Action_MatrixTMatrix<Interface> >(MIN_MM,MAX_MM, N);
    if (do_TriMatrixMatrix)
    bench<Action_TriMatrixMatrix<Interface> >(MIN_MM,MAX_MM, N);
    if (do_TriSolveMatrix)
    bench<Action_TriSolveMatrix<Interface> >(MIN_MM,MAX_MM, N);


  return 0;
}

