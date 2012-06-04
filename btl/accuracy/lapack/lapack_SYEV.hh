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
#ifndef LAPACK_SYEV_HH
#define LAPACK_SYEV_HH

#include "LinearCongruential.hh"
#include "diff.hh"

double test_SYEV(const int& N, const unsigned& seed = 0)
{
  LinearCongruential lc(seed);
  vector<double> A(N*N), Acopy, W(N), D(N*N), work(1), tmp(N*N);

  /* Fill A (symmetric) and Acopy */
  for (int r = 0; r < N; ++r) {
      A[r+r*N] = lc.get_01();
      for (int c = r+1; c < N; ++c) {
          A[r+c*N] = lc.get_01();
          A[c+r*N] = A[r+c*N];
      }
  }
  Acopy = A;

  /* Retrieve lwork */
  int lwork = -1, info;
  dsyev_("V", "U", &N, &A[0], &N, &W[0], &work[0], &lwork, &info);
  lwork = work[0];
  work.resize(lwork);

  /* Perform computation */
  dsyev_("V", "U", &N, &A[0], &N, &W[0], &work[0], &lwork, &info);

  /* Construct D */
  for (int i = 0; i < N; ++i)
    D[i+i*N] = W[i];

  /* A = V*D*V' */
  const double dONE = 1., dZERO = 0.;
  dgemm_("N", "N", &N, &N, &N, &dONE,   &A[0], &N, &D[0], &N, &dZERO, &tmp[0], &N);
  dgemm_("N", "T", &N, &N, &N, &dONE, &tmp[0], &N, &A[0], &N, &dZERO,   &D[0], &N);

  return diff(Acopy, D);
}

#endif /* LAPACK_SYEV_HH */
