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
