
#define PBLAS_PREFIX p
#define PBLAS_FUNC(NAME) CAT(CAT(CAT(PBLAS_PREFIX, SCALAR_PREFIX),NAME),_)

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
    blacs_pinfo_(&myid, &procnum);

    PBLAS_FUNC(gemv)(&notrans, &GlobalRows, &GlobalCols,
        &alpha, A, &iONE, &iONE, descA,
                x, &iONE, &iONE, descX, &iONE,
         &beta, y, &iONE, &iONE, descY, &iONE
    );

  }
};

