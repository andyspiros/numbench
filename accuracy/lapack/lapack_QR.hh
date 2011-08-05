#ifndef LAPACK_QR_HH
#define LAPACK_QR_HH

#include "LinearCongruential.hh"
#include "diff.hh"

double test_QR(const int& N, const unsigned& seed = 0)
{
  LinearCongruential lc(seed);
  vector<double> A(N*N), Acopy, tau(N), work(1), Q(N*N), H(N*N), tmp;

  /* Fill A and Acopy */
  for (int i = 0; i < N*N; ++i)
    A[i] = lc.get_01();
  Acopy = A;

  /* Retrieve lwork */
  int lwork = -1, info;
  dgeqrf_(&N, &N, &A[0], &N, &tau[0], &work[0], &lwork, &info);
  lwork = work[0];
  work.resize(lwork);

  /* Compute decomposition */
  dgeqrf_(&N, &N, &A[0], &N, &tau[0], &work[0], &lwork, &info);

  /* Compute Q */
  for (int i = 0; i < N; ++i)
    Q[i+i*N] = 1.;

  double alpha, t;
  const double dZERO = 0., dONE = 1.;
  const int iONE = 1, N2 = N*N;
  work.resize(N2);
  for (int i = 0; i < N; ++i) {
      /* Generate H_i */
      for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
          H[r+c*N] = (r == c);
      dcopy_(&N, &A[i*N], &iONE, &work[0], &iONE);
      for (int j = 0; j < i; ++j)
        work[j] = 0.;
      work[i] = 1.;
      t = -tau[i];
      dger_(&N, &N, &t, &work[0], &iONE, &work[0], &iONE, &H[0], &N);

      /* Multiply Q = Q*H_i */
      dsymm_("R", "U", &N, &N, &dONE, &H[0], &N, &Q[0], &N, &dZERO, &work[0], &N);
      dcopy_(&N2, &work[0], &iONE, &Q[0], &iONE);
  }

  /* Multiply */
  dtrmm_("R", "U", "N", "N", &N, &N, &dONE, &A[0], &N, &Q[0], &N);

  return diff(Acopy, Q);
}

#endif /* LAPACK_QR_HH */
