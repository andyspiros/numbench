#ifndef LAPACK_CHOLESKY_HH
#define LAPACK_CHOLESKY_HH

#include "LinearCongruential.hh"
#include "diff.hh"

double test_cholesky(const int& N, const unsigned& seed = 0)
{
  LinearCongruential lc(seed);

  vector<double> A(N*N), Acopy, test(N*N);

  /* Fill A (SPD), Acopy and test (identity) */
  for (int r = 0; r < N; ++r) {
      A[r+r*N] = N;
      test[r+r*N] = 1.;
      for (int c = r; c < N; ++c) {
        A[r+c*N] += lc.get_01();
        A[c+r*N] = A[r+c*N];
      }
  }
  Acopy = A;

  /* Compute decomposition */
  int info;
  dpotrf_("L", &N, &A[0], &N, &info);

  /* Test */
  const double alpha = 1.;
  dtrmm_("L", "L", "N", "N", &N, &N, &alpha, &A[0], &N, &test[0], &N);
  dtrmm_("R", "L", "T", "N", &N, &N, &alpha, &A[0], &N, &test[0], &N);

  return diff(Acopy, test);

}

#endif /* LAPACK_CHOLESKY_HH */
