#ifndef LAPACK_SVD_HH
#define LAPACK_SVD_HH

#include "LinearCongruential.hh"
#include "diff.hh"

double test_SVD(const int& N, const unsigned& seed = 0)
{
  LinearCongruential lc(seed);

  vector<double> A(N*N), Acopy, U(N*N), VT(N*N), S_(N), S(N*N);
  vector<double> work(1);

  /* Fill A and Acopy */
  for (int i = 0; i < N*N; ++i)
    A[i] = lc.get_01();
  Acopy = A;

  /* Retrieve lwork */
  int lwork = -1, info;
  dgesvd_("A", "A", &N, &N, &A[0], &N, &S_[0], &U[0], &N, &VT[0], &N, &work[0], &lwork, &info);
  lwork = work[0];
  work.resize(lwork);

  /* Compute decomposition */
  dgesvd_("A", "A", &N, &N, &A[0], &N, &S_[0], &U[0], &N, &VT[0], &N, &work[0], &lwork, &info);

  /* Construct S */
  for (int r = 0; r < N; ++r)
    S[r+r*N] = S_[r];

  /* Test */
  const double dONE = 1., dZERO = 0.;
  dgemm_("N", "N", &N, &N, &N, &dONE, &U[0], &N,  &S[0], &N, &dZERO, &A[0], &N);
  dgemm_("N", "N", &N, &N, &N, &dONE, &A[0], &N, &VT[0], &N, &dZERO, &S[0], &N);

  return diff(Acopy, S);
}

#endif /* LAPACK_SVD_HH */
