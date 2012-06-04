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
#ifndef CAT
#  define CAT2(A,B) A##B
#  define CAT(A,B) CAT2(A,B)
#endif
#define LAPACKFUNC(NAME) CAT(CAT(SCALAR_PREFIX,NAME),_)

#ifdef __cplusplus
extern "C" {
#endif

void LAPACKFUNC(getrf)(const int*, const int*, SCALAR*, const int*, int*, int*);
void LAPACKFUNC(potrf)(const char*, const int*, SCALAR*, const int*, int*);
void LAPACKFUNC(geqrf)(const int*, const int*, SCALAR*, const int*, SCALAR*, SCALAR*, const int*, int*);
void LAPACKFUNC(gesvd)(const char*, const char*, const int*, const int*, SCALAR*, const int*, SCALAR*, SCALAR*, const int*, SCALAR*, const int*, SCALAR*, const int*, int*);
void LAPACKFUNC(syev)(const char*, const char*, const int*, SCALAR*, const int*, SCALAR*, SCALAR*, const int*, int*);
void LAPACKFUNC(stev)(const char*, const int*, SCALAR*, SCALAR*, SCALAR*, const int*, SCALAR*, int*);

#ifdef __cplusplus
}
#endif

