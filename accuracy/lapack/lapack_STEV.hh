#ifndef LAPACK_STEV_HH
#define LAPACK_STEV_HH

#include "LinearCongruential.hh"
#include "diff.hh"

double test_STEV(const int& N, const unsigned& seed = 0)
{
  LinearCongruential lc(seed);
  vector<double> A(N*N), D(N), E(N-1), Z(N*N), DD(N*N), work(2*N-2), tmp(N*N);

  /* Fill D, E and A */
  for (int i = 0; i < N-1; ++i) {
      D[i] = lc.get_01(); A[i+i*N] = D[i];
      E[i] = lc.get_01(); A[(i+1)+i*N] = E[i]; A[i+(i+1)*N] = E[i];
  }
  D[N-1] = lc.get_01(); A[N*N-1] = D[N-1];

  /* Perform computation */
  int info;
  dstev_("V", &N, &D[0], &E[0], &Z[0], &N, &work[0], &info);

  /* Construct DD */
  for (int i = 0; i < N; ++i)
    DD[i+i*N] = D[i];

  /* A = V*D*V' */
  const double dONE = 1., dZERO = 0.;
  dgemm_("N", "N", &N, &N, &N, &dONE,   &Z[0], &N, &DD[0], &N, &dZERO, &tmp[0], &N);
  dgemm_("N", "T", &N, &N, &N, &dONE, &tmp[0], &N,  &Z[0], &N, &dZERO,  &DD[0], &N);

  return diff(A, DD);
}

#endif /* LAPACK_STEV_HH */
