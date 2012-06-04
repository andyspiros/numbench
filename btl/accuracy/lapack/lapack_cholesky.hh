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
