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


};
