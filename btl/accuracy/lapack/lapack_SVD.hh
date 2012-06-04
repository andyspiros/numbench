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
