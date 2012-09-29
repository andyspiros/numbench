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
#define NI_FORTRAN
#include "NumericInterface.hpp"

// Include the BTL
#include "utilities.h"
#include "bench.hh"

// Include the operations
#include "actionsBLAS.hpp"

#include <string>

BTL_MAIN;

int main(int argv, char **argc)
{
    bench<Action_axpy<NumericInterface<double> > >(4, 6000, 20);
    bench<Action_rot<NumericInterface<double> > >(4, 6000, 20);

    bench<Action_MatrixVector<NumericInterface<double> > >(4, 2000, 20);
    bench<Action_MatrixTVector<NumericInterface<double> > >(4, 2000, 20);
    bench<Action_SymMatrixVector<NumericInterface<double> > >(4, 2000, 20);
    bench<Action_TriSolveVector<NumericInterface<double> > >(4, 2000, 20);
    bench<Action_Rank1Update<NumericInterface<double> > >(4, 2000, 20);
    bench<Action_Rank2Update<NumericInterface<double> > >(4, 2000, 20);


  return 0;
}

