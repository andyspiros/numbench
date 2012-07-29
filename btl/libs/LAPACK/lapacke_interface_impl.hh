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
#define LAPACKEFUNC(NAME) CAT(LAPACKE_,CAT(SCALAR_PREFIX,NAME))
#define BLASFUNC(NAME) CAT(CAT(SCALAR_PREFIX,NAME),_)

#include "lapacke.h"


// Define BLAS functions used by LAPACKE interface
extern "C" {
    int BLASFUNC(copy) (int *, SCALAR*, int *, SCALAR*, int *);
}

template<> class lapack_interface<SCALAR> : public c_interface_base<SCALAR>
{
public:

	static inline std::string name()
	{
		return MAKE_STRING(LAPACKNAME);
	}

	static inline void general_solve(gene_matrix& A, gene_vector& b, gene_vector& x, int N)
	{
		std::vector<int> ipiv(N);
		BLASFUNC(copy)(&N, b, &intone, x, &intone);
		LAPACKEFUNC(gesv)(LAPACK_COL_MAJOR, N, 1, A, N, &ipiv[0], x, N);
	}

	static inline void least_squares(gene_matrix& A, gene_vector& b, gene_vector& x, int N)
	{
		BLASFUNC(copy)(&N, b, &intone, x, &intone);
		LAPACKEFUNC(gels)(LAPACK_COL_MAJOR, 'N', N, N, 1, A, N, x, N);
	}

	static inline void lu_decomp(const gene_matrix& X, gene_matrix& C, int N)
	{
		int N2 = N*N;
		std::vector<int> ipiv(N);
		BLASFUNC(copy)(&N2, X, &intone, C, &intone);
		LAPACKEFUNC(getrf)(LAPACK_COL_MAJOR, N, N, C, N, &ipiv[0]);
	}

	static inline void cholesky(const gene_matrix& X, gene_matrix& C, int N)
	{
		int N2 = N*N;
		BLASFUNC(copy)(&N2, X, &intone, C, &intone);
		LAPACKEFUNC(potrf)(LAPACK_COL_MAJOR, 'L', N, C, N);
	}
	
	static inline void qr_decomp(const gene_matrix& X, gene_matrix& QR, gene_vector& tau, const int& N)
	{
		int N2 = N*N;
		BLASFUNC(copy)(&N2, X, &intone, QR, &intone);
		LAPACKEFUNC(geqrf)(LAPACK_COL_MAJOR, N, N, QR, N, tau);
	}

	static inline void svd_decomp(const gene_matrix& X, gene_matrix& U, gene_vector& S, gene_matrix& VT, const int& N)
	{
		int N2 = N*N;
		stl_vector Xcopy(N2), superb(N-1);
		BLASFUNC(copy)(&N2, X, &intone, &Xcopy[0], &intone);
		LAPACKEFUNC(gesvd)(LAPACK_COL_MAJOR, 'A', 'A', N, N, &Xcopy[0], N, S, U, N, VT, N, &superb[0]);
	}

	static inline void syev(const gene_matrix& X, gene_matrix& V, gene_vector& W, const int& N)
	{
		int N2 = N*N;
		BLASFUNC(copy)(&N2, X, &intone, V, &intone);
		LAPACKEFUNC(syev)(LAPACK_COL_MAJOR, 'V', 'U', N, V, N, W);
	}

	/* Size of D, W: N; size of E: N-1, size of V: NxN */
	static inline void stev(const gene_vector& D, const gene_vector& E, gene_vector& W, gene_matrix& V, int N)
	{
		stl_vector E_(E, E+N-1);
		BLASFUNC(copy)(&N, D, &intone, W, &intone);
		LAPACKEFUNC(stev)(LAPACK_COL_MAJOR, 'V', N, W, &E_[0], V, N);
	}

	static inline void symm_ev(const gene_matrix& X, gene_vector& W, int N)
	{
		LAPACKEFUNC(syev)(LAPACK_COL_MAJOR, 'N', 'L', N, X, N, W);
	}

};
