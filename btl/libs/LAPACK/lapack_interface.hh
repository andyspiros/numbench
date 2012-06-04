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
#ifndef LAPACK_INTERFACE_HH
#define LAPACK_INTERFACE_HH

#include <../BLAS/c_interface_base.h>
#include <complex>
#include "lapack.hh"

extern "C" {
#include "../BLAS/blas.h"

//void sgesv_(int*, int*, float *, int*, int*, float *, int*, int*);
//void dgesv_(int*, int*, double*, int*, int*, double*, int*, int*);

void sgels_(char*, int*, int*, int*, float *, int*, float *, int*, float *, int*, int*);
void dgels_(char*, int*, int*, int*, double*, int*, double*, int*, double*, int*, int*);

//void sgetrf_(int*, int*, float *, int*, int*, int*);
//void dgetrf_(int*, int*, double*, int*, int*, int*);

//void spotrf_(char*, int*, float *, int*, int*);
//void dpotrf_(char*, int*, double*, int*, int*);

//void ssyev_(char*, char*, int*, float *, int*, float *, float *, int*, int*);
//void dsyev_(char*, char*, int*, double*, int*, double*, double*, int*, int*);
}


#define MAKE_STRING2(S) #S
#define MAKE_STRING(S) MAKE_STRING2(S)

#ifndef CAT
#  define CAT2(A,B) A##B
#  define CAT(A,B) CAT2(A,B)
#endif

template <typename real> class lapack_interface;


static char notrans = 'N';
static char trans = 'T';
static char nonunit = 'N';
static char lower = 'L';
static char right = 'R';
static char left = 'L';
static int intone = 1;
static int zeroint = 0;


#define SCALAR        float
#define SCALAR_PREFIX s
#include "lapack_interface_impl.hh"
#undef SCALAR
#undef SCALAR_PREFIX

#define SCALAR        double
#define SCALAR_PREFIX d
#include "lapack_interface_impl.hh"
#undef SCALAR
#undef SCALAR_PREFIX

#endif /* LAPACK_INTERFACE_HH */
