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
#define LAPACKFUNC(NAME) CAT(CAT(SCALAR_PREFIX,NAME),_)

#include "lapack_.hh"
#include "blas.h"

template<> class lapack_interface<SCALAR> : public c_interface_base<SCALAR>
{
public:

	static inline std::string name()
	{
		return MAKE_STRING(LAPACKNAME);
	}

	static inline void general_solve(gene_matrix& A, gene_vector& b, gene_vector& x, int N)
	{
		int *ipiv = new int[N];
		int info;
		LAPACKFUNC(copy)(&N, b, &intone, x, &intone);
		LAPACKFUNC(gesv)(&N, &intone, A, &N, ipiv, x, &N, &info);
		delete[] ipiv;
	}

	static inline void least_squares(gene_matrix& A, gene_vector& b, gene_vector& x, int N)
	{
		int *ipiv = new int[N];
		int info;
		LAPACKFUNC(copy)(&N, b, &intone, x, &intone);
		SCALAR work1;
		int MONE = -1;
		LAPACKFUNC(gels)(&notrans, &N, &N, &intone, A, &N, x, &N, &work1, &MONE, &info);
		int lwork = (int)work1;
		SCALAR *work2 = new SCALAR[lwork];
		LAPACKFUNC(gels)(&notrans, &N, &N, &intone, A, &N, x, &N, work2, &lwork, &info);
		delete[] work2;
		delete[] ipiv;
	}

	static inline void lu_decomp(const gene_matrix& X, gene_matrix& C, int N)
	{
		int N2 = N*N;
		int *ipiv = new int[N];
		int info;
		LAPACKFUNC(copy)(&N2, X, &intone, C, &intone);
		LAPACKFUNC(getrf)(&N, &N, C, &N, ipiv, &info);
		delete[] ipiv;
	}

	static inline void cholesky(const gene_matrix& X, gene_matrix& C, int N)
	{
		int N2 = N*N;
		int info;
		LAPACKFUNC(copy)(&N2, X, &intone, C, &intone);
		LAPACKFUNC(potrf)(&lower, &N, C, &N, &info);
	}
	
	static inline void qr_decomp(const gene_matrix& X, gene_matrix& QR, gene_vector& tau, const int& N)
	{
		int N2 = N*N;
		LAPACKFUNC(copy)(&N2, X, &intone, QR, &intone);
		
		SCALAR *work = new SCALAR;
		int lwork = -1, info;
		LAPACKFUNC(geqrf)(&N, &N, QR, &N, tau, work, &lwork, &info);
		lwork = *work;
		delete work;
		work = new SCALAR[lwork];
		LAPACKFUNC(geqrf)(&N, &N, QR, &N, tau, work, &lwork, &info);
		delete[] work;
	}

	static inline void svd_decomp(const gene_matrix& X, gene_matrix& U, gene_vector& S, gene_matrix& VT, const int& N)
	{
		int N2 = N*N;
		stl_vector Xcopy(N2);
		LAPACKFUNC(copy)(&N2, X, &intone, &Xcopy[0], &intone);

		stl_vector work(1);
		int lwork = -1, info;
		LAPACKFUNC(gesvd)("A", "A", &N, &N, &Xcopy[0], &N, S, U, &N, VT, &N, &work[0], &lwork, &info);
		lwork = work[0];
		work.resize(lwork);
		LAPACKFUNC(gesvd)("A", "A", &N, &N, &Xcopy[0], &N, S, U, &N, VT, &N, &work[0], &lwork, &info);
	}

	static inline void syev(const gene_matrix& X, gene_matrix& V, gene_vector& W, const int& N)
	{
		int N2 = N*N;
		LAPACKFUNC(copy)(&N2, X, &intone, V, &intone);

		stl_vector work(1);
		int lwork = -1, info;
		LAPACKFUNC(syev)("V", "U", &N, V, &N, W, &work[0], &lwork, &info);
		lwork = work[0];
		work.resize(lwork);
		LAPACKFUNC(syev)("V", "U", &N, V, &N, W, &work[0], &lwork, &info);
	}

	/* Size of D, W: N; size of E: N-1, size of V: NxN */
	static inline void stev(const gene_vector& D, const gene_vector& E, gene_vector& W, gene_matrix& V, const int& N)
	{
		int N0 = N;
		LAPACKFUNC(copy)(&N0, D, &intone, W, &intone);
		stl_vector E_(E, E+N-1), work(max(1, 2*N-2));

		int info;
		LAPACKFUNC(stev)("V", &N, W, &E_[0], V, &N, &work[0], &info);
	}

	static inline void symm_ev(const gene_matrix& X, gene_vector& W, int N)
	{
		char jobz = 'N';
		SCALAR *work = new SCALAR;
		int lwork = -1, info;
		LAPACKFUNC(syev)(&jobz, &lower, &N, X, &N, W, work, &lwork, &info);
		lwork = *work;
		delete work;
		work = new SCALAR[lwork];
		LAPACKFUNC(syev)(&jobz, &lower, &N, X, &N, W, work, &lwork, &info);
		delete[] work;
	}


};
