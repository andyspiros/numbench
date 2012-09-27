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
#ifndef NUMERICINTERFACE_HPP_
#define NUMERICINTERFACE_HPP_

#include "NI_internal/utils.hpp"
#include <string>

template<class Scalar> class NumericInterface;

#define NI_INCLUDE_OK

#ifdef NI_FORTRAN
#
#  include "NI_internal/FortranDeclarations.hpp"
#
#  define NI_SCALAR double
#  define NI_SCALARPREFIX d
#  include "NI_internal/FortranInterface.hpp"
#  undef NI_SCALAR
#  undef NI_SCALARPREFIX
#
#  define NI_SCALAR float
#  define NI_SCALARPREFIX s
#  include "NI_internal/FortranInterface.hpp"
#  undef NI_SCALAR
#  undef NI_SCALARPREFIX
#
#else
#  include "NI_internal/CDeclarations.hpp"
#
#  define NI_SCALAR double
#  define NI_SCALARPREFIX d
#  include "NI_internal/CInterface.hpp"
#  undef NI_SCALAR
#  undef NI_SCALARPREFIX
#
#  define NI_SCALAR float
#  define NI_SCALARPREFIX s
#  include "NI_internal/CInterface.hpp"
#  undef NI_SCALAR
#  undef NI_SCALARPREFIX
#
#endif

#undef NI_INCLUDE_OK


#endif /* NUMERICINTERFACE_HPP_ */
