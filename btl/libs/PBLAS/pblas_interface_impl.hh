
#define PBLAS_PREFIX p
#define PBLAS_FUNC(NAME) CAT(CAT(CAT(PBLAS_PREFIX, SCALAR_PREFIX),NAME),_)
#define BLAS_FUNC(NAME) CAT(CAT(SCALAR_PREFIX,NAME),_)

template<> class pblas_interface<SCALAR> : public blacs_interface<SCALAR>
{
public:
  static inline std::string name()
  {
    return MAKE_STRING(PBLASNAME);
  }

  static inline void parallel_axpy(const SCALAR& coef,
      gene_vector& x, int *descX,
      gene_vector& y, int *descY,
      const int& size
      )
  {
    int iZERO = 0, iONE = 1;
    PBLAS_FUNC(axpy)(&size, &coef,
        x, &iONE, &iONE, descX, &iONE,
        y, &iONE, &iONE, descY, &iONE
    );
  }

  static inline void parallel_matrix_vector_product(
      int GlobalRows, int GlobalCols,
      gene_matrix& A, int *descA,
      gene_vector& x, int *descX,
      gene_vector& y, int *descY
      )
  {
    real_type alpha = 1., beta = 0.;
    int iONE = 1;
    int myid, procnum;
    const char notrans = 'N';
    blacs_pinfo_(&myid, &procnum);

    PBLAS_FUNC(gemv)(&notrans, &GlobalRows, &GlobalCols,
        &alpha, A, &iONE, &iONE, descA,
                x, &iONE, &iONE, descX, &iONE,
         &beta, y, &iONE, &iONE, descY, &iONE
    );

  }


  static inline void parallel_lu_decomp(gene_matrix& X, std::vector<int>& ipiv, const int* desc)
  {
    const int GlobalRows = desc[2], GlobalCols = desc[3];
    const int iONE = 1;
    int info;
    ipiv.resize(desc[8] + desc[4]);
    PBLAS_FUNC(getrf)(&GlobalRows, &GlobalCols, X, &iONE, &iONE, desc,
        &ipiv[0], &info);
    if (info != 0 && myid() == 0)
      cout << " { LU error : " << info << " } ";
  }

  static inline void parallel_cholesky(gene_matrix& X, const int* desc)
  {
    const int N = desc[2], iONE = 1;
    const char UP = 'U';
    int info;
    PBLAS_FUNC(potrf)(&UP, &N, X, &iONE, &iONE, desc, &info);
    if (info != 0 && myid() == 0)
      cout << " { cholesky error : " << info << " } ";
  }

  static inline void parallel_qr_decomp(gene_matrix& X, const int* desc)
  {
    const int GlobalRows = desc[2], GlobalCols = desc[3],
              BlockRows = desc[4], BlockCols = desc[5],
              ctxt = desc[1];

    int myrow, mycol, nprow, npcol, lwork;
    SCALAR lworkd;
    blacs_gridinfo_(&ctxt, &nprow, &npcol, &myrow, &mycol);

    const int iONE = 1, iZERO = 0, imONE = -1,
             ipivdim = numroc_(&GlobalCols, &BlockCols, &mycol, &iZERO, &npcol);
    int info;
    std::vector<int> ipiv(ipivdim);
    std::vector<SCALAR> tau(ipivdim);

    // Retrieve LWORK
    PBLAS_FUNC(geqpf)(&GlobalRows, &GlobalCols, X, &iONE, &iONE, desc, &ipiv[0], &tau[0], &lworkd, &imONE, &info);
    lwork = static_cast<int>(lworkd);
    if (info != 0 && myid() == 0)
      cout << " { qr_decomp lwork error } ";

    std::vector<SCALAR> work(lwork);
    PBLAS_FUNC(geqpf)(&GlobalRows, &GlobalCols, X, &iONE, &iONE, desc, &ipiv[0], &tau[0], &work[0], &lwork, &info);
    if (info != 0 && myid() == 0)
      cerr << " { qr_decomp computation error } ";
  }

  static inline void parallel_symm_ev(gene_matrix& A, const int* descA, gene_vector& w, gene_matrix& Z, const int* descZ)
  {
    const char jobz = 'V', uplo = 'u';
    const int N = descA[2], iONE = 1, iZERO = 0, imONE = -1;
    std::vector<SCALAR> work;
    std::vector<int> iwork;
    int lwork, liwork, info;
    SCALAR lworkd;

    // Retrieve l(i)work
    PBLAS_FUNC(syevd)(&jobz, &uplo, &N, A, &iONE, &iONE, descA, w,
        Z, &iONE, &iONE, descZ, &lworkd, &imONE, &liwork, &imONE, &info);
    lwork = static_cast<int>(lworkd);
    work.resize(lwork); iwork.resize(liwork);
    if (info != 0 && myid() == 0)
      cout << " { symm_ev l(i)work error } ";

    PBLAS_FUNC(syevd)(&jobz, &uplo, &N, A, &iONE, &iONE, descA, w,
        Z, &iONE, &iONE, descZ, &work[0], &lwork, &iwork[0], &liwork, &info);
    if (info != 0 && myid() == 0)
      cout << " { symm_ev computation error } ";
  }

  static inline void parallel_svd_decomp(gene_matrix& A, int* descA, gene_matrix& U, int *descU, gene_matrix& V, int *descV, gene_vector& s)
  {
    const char job = 'V';
    const int size = descA[2], iONE = 1, iZERO = 0, imONE = -1;
    std::vector<SCALAR> work;
    int info, lwork;
    SCALAR lworkd;

    // Retrieve lwork
    PBLAS_FUNC(gesvd)(&job, &job, &size, &size, A, &iONE, &iONE, descA, s,
        U, &iONE, &iONE, descU, V, &iONE, &iONE, descV, &lworkd, &imONE, &info);
    if (info != 0 && myid() == 0)
      cout << " { svd_decomp lwork error } ";
    lwork = static_cast<int>(lworkd);
    work.resize(lwork);

    PBLAS_FUNC(gesvd)(&job, &job, &size, &size, A, &iONE, &iONE, descA, s,
        U, &iONE, &iONE, descU, V, &iONE, &iONE, descV, &work[0], &lwork, &info);
    if (info != 0 && myid() == 0)
      cout << " { svd_decomp computation error } ";
  }

  static inline real_type
    test_LU(stl_matrix& Global_A, gene_matrix& Local_LU, int *desc)
    {
      bool iamroot = myid() == 0;
      int _size = desc[2];

      // Create and scatter Identity
      int Testdesc[9];
      stl_matrix Global_Test_stl, Local_Test_stl;
      if (iamroot)
        {
          stl_matrix Identity(_size * _size);
          for (int r = 0; r < _size; ++r)
            Identity[r + _size * r] = 1;
          scatter_matrix(Identity, Local_Test_stl, Testdesc, _size, _size,
              desc[4], desc[5]);
        }
      else
        scatter_matrix(stl_matrix(), Local_Test_stl, Testdesc);

      // Compute L * U
      real_type alpha = 1., malpha = -1;
      int iONE = 1;
      PBLAS_FUNC(trmm)("L", "L", "N", "N", desc + 2, desc + 2, &alpha, Local_LU,
          &iONE, &iONE, desc, &Local_Test_stl[0], &iONE, &iONE, Testdesc);
      PBLAS_FUNC(trmm)("R", "U", "N", "N", desc + 2, desc + 2, &alpha, Local_LU,
          &iONE, &iONE, desc, &Local_Test_stl[0], &iONE, &iONE, Testdesc);

      // Gather result
      gather_matrix(Global_Test_stl, Local_Test_stl, desc);
      if (iamroot)
        {
          int size2 = _size*_size;
          BLAS_FUNC(axpy)(&size2, &malpha, &Global_A[0], &iONE,
              &Global_Test_stl[0], &iONE);
          double error = BLAS_FUNC(nrm2)(&size2, &Global_Test_stl[0], &iONE);
          error /= BLAS_FUNC(nrm2)(&size2, &Global_A[0], &iONE);
          return error;
        }
      else
        return 0.;
  }

  static inline real_type
    test_cholesky(stl_matrix& Global_A, gene_matrix& Local_U, int *desc)
    {
      bool iamroot = myid() == 0;
      int _size = desc[2];

      // Create and scatter Identity
      int Testdesc[9];
      stl_matrix Global_Test_stl, Local_Test_stl;
      if (iamroot)
        {
          stl_matrix Identity(_size * _size);
          for (int r = 0; r < _size; ++r)
            Identity[r + _size * r] = 1;
          scatter_matrix(Identity, Local_Test_stl, Testdesc, _size, _size,
              desc[4], desc[5]);
        }
      else
        scatter_matrix(stl_matrix(), Local_Test_stl, Testdesc);

      // Compute U' * U
      real_type alpha = 1., malpha = -1;
      int iONE = 1;
      PBLAS_FUNC(trmm)("L", "U", "T", "N", desc + 2, desc + 2, &alpha, Local_U,
          &iONE, &iONE, desc, &Local_Test_stl[0], &iONE, &iONE, Testdesc);
      PBLAS_FUNC(trmm)("R", "U", "N", "N", desc + 2, desc + 2, &alpha, Local_U,
          &iONE, &iONE, desc, &Local_Test_stl[0], &iONE, &iONE, Testdesc);

      // Gather result
      gather_matrix(Global_Test_stl, Local_Test_stl, desc);
      if (iamroot)
        {
          int size2 = _size*_size;
          BLAS_FUNC(axpy)(&size2, &malpha, &Global_A[0], &iONE,
              &Global_Test_stl[0], &iONE);
          double error = BLAS_FUNC(nrm2)(&size2, &Global_Test_stl[0], &iONE);
          error /= BLAS_FUNC(nrm2)(&size2, &Global_A[0], &iONE);
          return error;
        }
      else
        return 0.;
  }
};
