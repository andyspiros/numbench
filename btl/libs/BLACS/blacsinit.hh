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
#ifndef BLACSINIT_HH
#define BLACSINIT_HH

#include <cmath>

bool blacsinit(int *argc, char ***argv)
{
  int context, myid, numproc, prows, pcols, iZERO = 0;
  MPI_Init(argc, argv);
  blacs_pinfo_(&myid, &numproc);
  blacs_get_(&iZERO, &iZERO, &context);

  int p = static_cast<double>(std::sqrt(static_cast<double>(numproc)) + 1.);
  while (numproc % p) --p;
  prows = p; pcols = numproc/p;

  blacs_gridinit_(&context, "Row-major", &prows, &pcols);
  return (myid == 0);
}

#endif /* BLACSINIT_HH */
