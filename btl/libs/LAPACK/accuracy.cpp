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

#include <string>
#include <iostream>

// Include the numeric interface
#ifdef NI_LAPACK
#  define NI_FORTRAN
#endif

#include "NumericInterface.hpp"

typedef NumericInterface<double> Interface;

// Include the BTL
#include "utilities.h"
#include "bench.hh"
#include "accuracy.hpp"

// Include the operations
#include "actionsLAPACK.hpp"


using namespace std;

BTL_MAIN;

int main(int argv, char **argc)
{
    bool
    do_GeneralSolve=false, do_LeastSquaresSolve=false,

    do_LUdecomp=false, do_Choleskydecomp=false, do_QRdecomp=false
    ;

    int N = 100;


    for (int i = 1; i < argv; ++i) {
      std::string arg = argc[i];

           if (arg == "GeneralSolve") do_GeneralSolve = true;
      else if (arg == "LeastSquaresSolve") do_LeastSquaresSolve = true;

      else if (arg == "LUdecomp") do_LUdecomp= true;
      else if (arg == "Choleskydecomp") do_Choleskydecomp= true;
      else if (arg == "QRdecomp") do_QRdecomp= true;

      // Check switch -N
      else if (arg[0] == '-' && arg[1] == 'N') {
          if (arg[2] != '\0')
              N = atoi(arg.c_str()+2);
          else
              N = atoi(argc[++i]);
      }
    }

    if (do_GeneralSolve)
    bench_accuracy<Action_GeneralSolve<Interface> >(MIN_MM,MAX_MM, N);
    if (do_LeastSquaresSolve)
    bench_accuracy<Action_LeastSquaresSolve<Interface> >(MIN_MM,MAX_MM, N);

    if(do_LUdecomp)
    bench_accuracy<Action_LUdecomp<Interface> >(MIN_MM,MAX_MM, N);
    if(do_Choleskydecomp)
    bench_accuracy<Action_Choleskydecomp<Interface> >(MIN_MM,MAX_MM, N);
    if(do_QRdecomp)
    bench_accuracy<Action_QRdecomp<Interface> >(MIN_MM,MAX_MM, N);


    return 0;
}

