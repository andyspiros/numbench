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
#  error FortranInterface.hpp should never be included directly: include NumericInterface.hpp instead!
#endif


#define FORTFUNC(F) CAT(CAT(NI_SCALARPREFIX, F), _)


template<>
class NumericInterface<NI_SCALAR>
{
public:
    typedef NI_SCALAR Scalar;

    static const int ZERO;
    static const int ONE;

public:
    static std::string name()
    {
        std::string name = "FortranInterface<";
        name += MAKE_STRING(NI_SCALAR);
        name += ">";
        return name;
    }



    /****************
     * LEVEL 1 BLAS *
     ****************/

    static void rot(const int& N, Scalar* x, Scalar* y,
                    const Scalar& cosine, const Scalar& sine)
    {
        FORTFUNC(rot)(&N, x, &ONE, y, &ONE, &cosine, &sine);
    }


    static void axpy(const int& N, const Scalar& alpha,
                     const Scalar* x, Scalar* y)
    {
        FORTFUNC(axpy)(&N, &alpha, x, &ONE, y, &ONE);
    }

    static Scalar dot(const int& N, const Scalar* x, const Scalar* y)
    {
        return FORTFUNC(dot)(&N, x, &ONE, y, &ONE);
    }

    static Scalar norm(const int& N, const Scalar* x)
    {
        return FORTFUNC(nrm2)(&N, x, &ONE);
    }



    /****************
     * LEVEL 2 BLAS *
     ****************/

    static void matrixVector(const char& trans, const int& M, const int& N,
            const Scalar& alpha, const Scalar* A, const Scalar* x,
            const Scalar& beta, Scalar* y)
    {
        FORTFUNC(gemv)(&trans, &M, &N, &alpha, A, &M, x, &ONE, &beta, y, &ONE);
    }

    static void triangularSolveVector(const char& uplo, const char& diag,
            const int& N, const Scalar* A, Scalar* x)
    {
        FORTFUNC(trsv)(&uplo, "N", &diag, &N, A, &N, x, &ONE);
    }

    static void rank1update(const int& M, const int& N, const Scalar& alpha,
            const Scalar* x, const Scalar* y, Scalar* A)
    {
        FORTFUNC(ger)(&M, &N, &alpha, x, &ONE, y, &ONE, A, &M);
    }
};

const int NumericInterface<NI_SCALAR>::ZERO = 0;
const int NumericInterface<NI_SCALAR>::ONE = 1;










