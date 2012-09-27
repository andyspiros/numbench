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

/* Check whether we are included by NumeriInterface.hpp or not */
#ifndef NI_INCLUDE_OK
#  error CInterface.hpp should never be included directly: include NumericInterface.hpp instead!
#endif

#define CBLASFUNC(F) CAT(cblas_, CAT(NI_SCALARPREFIX, F))

template<>
class NumericInterface<NI_SCALAR>
{
public:
    typedef NI_SCALAR Scalar;

public:
    static std::string name()
    {
        std::string name = "CInterface<";
        name += MAKE_STRING(NI_SCALAR);
        name += ">";
        return name;
    }


    static void matrixVector(
            const int& M, const int& N, const Scalar& alpha, const Scalar* A,
            const Scalar* x, const Scalar& beta, Scalar* y
        )
    {
        CBLASFUNC(gemv)(CblasColMajor, CblasNoTrans, M, N, alpha, A, M,
                        x, 1, beta, y, 1);
    }
};
