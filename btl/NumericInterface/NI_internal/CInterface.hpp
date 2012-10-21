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
#define LAPACKEFUNC(F) CAT(LAPACKE_, CAT(NI_SCALARPREFIX, F))

#ifndef NI_NAME
#  define NI_NAME CAT(CAT(CInterface,$),NI_SCALAR)
#endif


template<>
class NumericInterface<NI_SCALAR>
{
public:
    typedef NI_SCALAR Scalar;

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
        CBLASFUNC(rot)(N, x, 1, y, 1, cosine, sine);
    }


    static void axpy(const int& N, const Scalar& alpha,
                     const Scalar* x, Scalar* y)
    {
        CBLASFUNC(axpy)(N, alpha, x, 1, y, 1);
    }

    static Scalar dot(const int& N, const Scalar* x, const Scalar* y)
    {
        return CBLASFUNC(dot)(N, x, 1, y, 1);
    }

    static Scalar norm(const int& N, const Scalar* x)
    {
        return CBLASFUNC(nrm2)(N, x, 1);
    }



    /****************
     * LEVEL 2 BLAS *
     ****************/

    static void MatrixVector(const bool& trans, const int& M, const int& N,
            const Scalar& alpha, const Scalar* A, const Scalar* x,
            const Scalar& beta, Scalar* y)
    {
        const int LDA = trans ? N : M;
        const int tA = trans ? CblasTrans : CblasNoTrans;
        CBLASFUNC(gemv)(CblasColMajor, tA, M, N, alpha, A, LDA,
                        x, 1, beta, y, 1);
    }

    static void SymMatrixVector(const char& uplo, const int& N,
            const Scalar& alpha, const Scalar* A, const Scalar* x,
            const Scalar& beta, Scalar* y)
    {
        int uplo_ = -1;
        if (uplo == 'u' || uplo == 'U')
            uplo_ = CblasUpper;
        else if (uplo == 'l' || uplo == 'L')
            uplo_ = CblasLower;

        CBLASFUNC(symv)(CblasColMajor, uplo_, N, alpha, A, N, x, 1, beta, y, 1);
    }

    static void TriMatrixVector(const char& uplo, const int& N,
            const Scalar* A, Scalar* x)
    {
        int uplo_ = -1;
        if (uplo == 'u' || uplo == 'U')
            uplo_ = CblasUpper;
        else if (uplo == 'l' || uplo == 'L')
            uplo_ = CblasLower;

        CBLASFUNC(trmv)(CblasColMajor, uplo_, CblasNoTrans, CblasNonUnit, N,
                        A, N, x, 1);
    }

    static void TriSolveVector(const char& uplo,
            const int& N, const Scalar* A, Scalar* x)
    {
        int uplo_ = -1;
        if (uplo == 'u' || uplo == 'U')
            uplo_ = CblasUpper;
        else if (uplo == 'l' || uplo == 'L')
            uplo_ = CblasLower;

        CBLASFUNC(trsv)(CblasColMajor, uplo_, CblasNoTrans, CblasNonUnit, N,
                A, N, x, 1);
    }

    static void Rank1Update(const int& M, const int& N, const Scalar& alpha,
            const Scalar* x, const Scalar* y, Scalar* A)
    {
        CBLASFUNC(ger)(CblasColMajor, M, N, alpha, x, 1, y, 1, A, M);
    }

    static void Rank2Update(const char& uplo, const int& N, const Scalar& alpha,
            const Scalar* x, const Scalar* y, Scalar* A)
    {
        int uplo_ = -1;
        if (uplo == 'u' || uplo == 'U')
            uplo_ = CblasUpper;
        else if (uplo == 'l' || uplo == 'L')
            uplo_ = CblasLower;

        CBLASFUNC(syr2)(CblasColMajor, uplo_, N, alpha, x, 1, y, 1, A, N);
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
        int tA = CblasNoTrans, tB = CblasNoTrans;

        if (transA) {
            LDA = K;
            tA = CblasTrans;
        }
        if (transB) {
            LDB = N;
            tB = CblasTrans;
        }

        CBLASFUNC(gemm)(CblasColMajor, tA, tB, M, N, K, alpha, A, LDA, B, LDB,
                        beta, C, M);
    }

    static void TriMatrixMatrix(const char& uplo,
            const int& M, const int& N, const Scalar* A, Scalar* B)
    {
        int uplo_ = -1;
        if (uplo == 'u' || uplo == 'U')
            uplo_ = CblasUpper;
        else if (uplo == 'l' || uplo == 'L')
            uplo_ = CblasLower;

        CBLASFUNC(trmm)(CblasColMajor, CblasLeft, uplo_, CblasNoTrans,
                        CblasNonUnit, M, N, 1., A, M, B, M);
    }

    static void TriSolveMatrix(const char& uplo,
            const int& M, const int& N, const Scalar* A, Scalar *B)
    {
        int uplo_ = -1;
        if (uplo == 'u' || uplo == 'U')
            uplo_ = CblasUpper;
        else if (uplo == 'l' || uplo == 'L')
            uplo_ = CblasLower;

        CBLASFUNC(trsm)(CblasColMajor, CblasLeft, uplo_, CblasNoTrans,
                CblasNonUnit, M, N, 1., A, M, B, M);
    }



    /*******************
     * LAPACKE SOLVERS *
     *******************/

    static void GeneralSolve(const int& N, Scalar *A, Scalar *b, int *ipiv)
    {
        LAPACKEFUNC(gesv)(CblasColMajor, N, 1, A, N, ipiv, b, N);
    }

    static void LeastSquaresSolve(const int& N, Scalar *A, Scalar *b)
    {
        LAPACKEFUNC(gels)(CblasColMajor, 'N', N, N, 1, A, N, b, N);
    }



    /**************************
     * LAPACKE decompositions *
     **************************/

    static void LUdecomp(const int& N, Scalar* A, int* ipiv)
    {
        LAPACKEFUNC(getrf)(CblasColMajor, N, N, A, N, ipiv);
    }

    static void Choleskydecomp(const char& uplo, const int& N, Scalar* A)
    {
        LAPACKEFUNC(potrf)(CblasColMajor, uplo, N, A, N);
    }

    static void QRdecomp(const int& N, Scalar* A, int* jpiv, Scalar* tau)
    {
        LAPACKEFUNC(geqp3)(CblasColMajor, N, N, A, N, jpiv, tau);
    }
};
