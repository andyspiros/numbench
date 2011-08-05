#ifndef LAPACK_LU_HH
#define LAPACK_LU_HH

#include "LinearCongruential.hh"
#include "diff.hh"

vector<int> get_piv(const vector<int>& ipiv)
{
  const int size = ipiv.size();
  vector<int> ret(size);
  for (int i = 0; i < size; ++i)
    ret[i] = i;

  int ii, jj, tmp;
  for (int i = 1; i <= size; ++i) {
      ii = i-1;
      jj = ipiv[ii]-1;
      tmp = ret[ii];
      ret[ii] = ret[jj];
      ret[jj] = tmp;
  }

  return ret;
}

double test_LU(const int& N, const unsigned& seed = 0)
{
  LinearCongruential lc(seed);

  vector<double> A(N*N), Acopy, P(N*N);

  /* Fill A and Acopy */
  for (int i = 0; i < N*N; ++i)
    A[i] = lc.get_01();
  Acopy = A;

  /* Compute decomposition */
  vector<int> ipiv(N);
  int info;
  dgetrf_(&N, &N, &A[0], &N, &ipiv[0], &info);
  vector<int> piv = get_piv(ipiv);

  /* Construct P */
  for (int r = 0; r < N; ++r) {
    int c = piv[r];
    P[c+N*r] = 1;
  }

  /* Test */
  const double alpha = 1.;
  dtrmm_("R", "L", "N", "U", &N, &N, &alpha, &A[0], &N, &P[0], &N);
  dtrmm_("R", "U", "N", "N", &N, &N, &alpha, &A[0], &N, &P[0], &N);

  return diff(Acopy, P);
}

#endif /* LAPACK_LU_HH */
