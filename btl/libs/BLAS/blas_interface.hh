//=====================================================
// File   :  blas_interface.hh
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
#ifndef blas_PRODUIT_MATRICE_VECTEUR_HH
#define blas_PRODUIT_MATRICE_VECTEUR_HH

#include <c_interface_base.h>
#include <complex>
extern "C"
{
#include "blas.h"
#ifdef CBLAS_INTERFACE
#  include "cblas.h"
#endif
}

#define MAKE_STRING2(S) #S
#define MAKE_STRING(S) MAKE_STRING2(S)

#define CAT2(A,B) A##B
#define CAT(A,B) CAT2(A,B)


template<class real> class blas_interface;


static char notrans = 'N';
static char trans = 'T';
static char nonunit = 'N';
static char lower = 'L';
static char right = 'R';
static char left = 'L';
static int intone = 1;


#define SCALAR        float
#define SCALAR_PREFIX s
#ifdef CBLAS_INTERFACE
#  include "cblas_interface_impl.hh"
#else
#  include "blas_interface_impl.hh"
#endif
#undef SCALAR
#undef SCALAR_PREFIX


#define SCALAR        double
#define SCALAR_PREFIX d
#ifdef CBLAS_INTERFACE
#  include "cblas_interface_impl.hh"
#else
#  include "blas_interface_impl.hh"
#endif
#undef SCALAR
#undef SCALAR_PREFIX

#endif



