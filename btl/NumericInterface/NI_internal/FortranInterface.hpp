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

#ifndef NI_NAME
#  define NI_NAME CAT(CAT(FortranInterface,$),NI_SCALAR)
#endif

#include <vector>

template<>
class NumericInterface<NI_SCALAR>
{
public:
    typedef NI_SCALAR Scalar;

    static const int ZERO;
    static const int ONE;
    static const Scalar fONE;
    static const char NoTrans;
    static const char Trans;

public:
    static std::string name()
    {
        std::string name = MAKE_STRING(NI_NAME);
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

    static void MatrixVector(const bool& trans, const int& M, const int& N,
            const Scalar& alpha, const Scalar* A, const Scalar* x,
            const Scalar& beta, Scalar* y)
    {
        const int LDA = trans ? N : M;
        const char tA = trans ? Trans : NoTrans;
        FORTFUNC(gemv)(&tA, &M, &N, &alpha, A, &LDA, x, &ONE, &beta, y, &ONE);
    }

    static void SymMatrixVector(const char& uplo, const int& N,
            const Scalar& alpha, const Scalar* A, const Scalar* x,
            const Scalar& beta, Scalar* y)
    {
        FORTFUNC(symv)(&uplo, &N, &alpha, A, &N, x, &ONE, &beta, y, &ONE);
    }

    static void TriMatrixVector(const char& uplo, const int& N,
            const Scalar* A, Scalar* x)
    {
        FORTFUNC(trmv)(&uplo, "N", "N", &N, A, &N, x, &ONE);
    }

    static void TriSolveVector(const char& uplo,
            const int& N, const Scalar* A, Scalar* x)
    {
        FORTFUNC(trsv)(&uplo, "N", "N", &N, A, &N, x, &ONE);
    }

    static void Rank1Update(const int& M, const int& N, const Scalar& alpha,
            const Scalar* x, const Scalar* y, Scalar* A)
    {
        FORTFUNC(ger)(&M, &N, &alpha, x, &ONE, y, &ONE, A, &M);
    }

    static void Rank2Update(const char& uplo, const int& N, const Scalar& alpha,
            const Scalar* x, const Scalar* y, Scalar* A)
    {
        FORTFUNC(syr2)(&uplo, &N, &alpha, x, &ONE, y, &ONE, A, &N);
    }



    /****************
     * LEVEL 3 BLAS *
     ****************/

    static void MatrixMatrix(const bool& transA, const bool& transB,
            const int& M, const int& N, const int& K,
            const Scalar& alpha, const Scalar* A, const Scalar* B,
            const Scalar& beta, Scalar* C)
    {
        int LDA = M, LDB = K;
        char tA = NoTrans, tB = NoTrans;

        if (transA) {
            LDA = K;
            tA = Trans;
        }
        if (transB) {
            LDB = N;
            tB = Trans;
        }

        FORTFUNC(gemm)(&tA, &tB, &M, &N, &K, &alpha, A, &LDA, B, &LDB,
                       &beta, C, &M);
    }

    static void TriMatrixMatrix(const char& uplo,
            const int& M, const int& N, const Scalar* A, Scalar* B)
    {
        FORTFUNC(trmm)("L", &uplo, "N", "N", &M, &N, &fONE, A, &M, B, &M);
    }

    static void TriSolveMatrix(const char& uplo,
            const int& M, const int& N, const Scalar* A, Scalar *B)
    {
        FORTFUNC(trsm)("L", &uplo, "N", "N", &M, &N, &fONE, A, &M, B, &M);
    }



    /******************
     * LAPACK SOLVERS *
     ******************/

    static void GeneralSolve(const int& N, Scalar *A, Scalar *b, int *ipiv)
    {
        int info;
        FORTFUNC(gesv)(&N, &ONE, A, &N, ipiv, b, &N, &info);
    }

    static void LeastSquaresSolve(const int& N, Scalar *A, Scalar *b)
    {
        int lw, info;
        const int mONE = -1;
        Scalar lworks;

        FORTFUNC(gels)("N", &N, &N, &ONE, A, &N, b, &N, &lworks, &mONE, &info);
        lw = static_cast<int>(lworks);
        std::vector<Scalar> work(lw);
        FORTFUNC(gels)("N", &N, &N, &ONE, A, &N, b, &N, &work[0], &lw, &info);
    }



    /******************
     * LAPACK SOLVERS *
     ******************/

    static void LUdecomp(const int& N, Scalar* A, int* ipiv)
    {
        int info;
        FORTFUNC(getrf)(&N, &N, A, &N, ipiv, &info);
        if (info != 0) std::cerr << "Error: " << info << std::endl;
    }

    static void Choleskydecomp(const char& uplo, const int& N, Scalar* A)
    {
        int info;
        FORTFUNC(potrf)(&uplo, &N, A, &N, &info);
        if (info != 0) std::cerr << "Error: " << info << std::endl;
    }
};

const int NumericInterface<NI_SCALAR>::ZERO = 0;
const int NumericInterface<NI_SCALAR>::ONE = 1;
const NI_SCALAR NumericInterface<NI_SCALAR>::fONE = 1.;
const char NumericInterface<NI_SCALAR>::NoTrans = 'N';
const char NumericInterface<NI_SCALAR>::Trans = 'T';
