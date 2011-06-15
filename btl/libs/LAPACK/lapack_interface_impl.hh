#define LPF(NAME) CAT(CAT(SCALAR_PREFIX,NAME),_)

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
		LPF(copy)(&N, b, &intone, x, &intone);
		LPF(gesv)(&N, &intone, A, &N, ipiv, x, &N, &info);
		delete[] ipiv;
	}

	static inline void least_squares(gene_matrix& A, gene_vector& b, gene_vector& x, int N)
	{
		int *ipiv = new int[N];
		int info;
		LPF(copy)(&N, b, &intone, x, &intone);
		SCALAR work1;
		int MONE = -1;
		LPF(gels)(&notrans, &N, &N, &intone, A, &N, x, &N, &work1, &MONE, &info);
		int lwork = (int)work1;
		SCALAR *work2 = new SCALAR[lwork];
		LPF(gels)(&notrans, &N, &N, &intone, A, &N, x, &N, work2, &lwork, &info);
		delete[] work2;
		delete[] ipiv;
	}

	static inline void lu_decomp(const gene_matrix& X, gene_matrix& C, int N)
	{
		int N2 = N*N;
		int *ipiv = new int[N];
		int info;
		LPF(copy)(&N2, X, &intone, C, &intone);
		LPF(getrf)(&N, &N, C, &N, ipiv, &info);
		delete[] ipiv;
	}

	static inline void cholesky(const gene_matrix& X, gene_matrix& C, int N)
	{
		int N2 = N*N;
		int info;
		LPF(copy)(&N2, X, &intone, C, &intone);
		LPF(potrf)(&lower, &N, C, &N, &info);
	}

	static inline void symm_ev(const gene_matrix& X, gene_vector& W, int N)
	{
		char jobz = 'N';
		SCALAR *work = new SCALAR;
		int lwork = -1, info;
		LPF(syev)(&jobz, &lower, &N, X, &N, W, work, &lwork, &info);
		lwork = *work;
		delete work;
		work = new SCALAR[lwork];
		LPF(syev)(&jobz, &lower, &N, X, &N, W, work, &lwork, &info);
		delete[] work;
	}


};
