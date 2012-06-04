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
#ifndef GATHER_H_
#define GATHER_H_

#define TYPENAME float
#define TYPEPREFIX s
#include "gather_impl.h"
#undef TYPENAME
#undef TYPEPREFIX

#define TYPENAME double
#define TYPEPREFIX d
#include "gather_impl.h"
#undef TYPENAME
#undef TYPEPREFIX

template<typename T>
static void gather_matrix(std::vector<T>& GlobalMatrix, const std::vector<T>& LocalMatrix,
    const int* desc
) {
  int GlobalRows = desc[2], GlobalCols = desc[3],
      BlockRows = desc[4], BlockCols = desc[5],
      LocalRows = desc[8], LocalCols = LocalMatrix.size()/desc[8];
  int ctxt;
  {
    int ignored, what = 0;
    blacs_get_(&ignored, &what, &ctxt);
  }
  gather(ctxt, GlobalMatrix, LocalMatrix, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);
}


#endif /* GATHER_H_ */
