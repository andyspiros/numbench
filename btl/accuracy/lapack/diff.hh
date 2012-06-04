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
#ifndef DIFF_HH
#define DIFF_HH

#include <vector>

double diff(const int& N, const double* x, const double* test)
{
  std::vector<double> d(x, x+N);
  const int iONE = 1;
  const double alpha = -1.;
  daxpy_(&N, &alpha, test, &iONE, &d[0], &iONE);
  return dnrm2_(&N, &d[0], &iONE)/dnrm2_(&N, x, &iONE);
}

template<typename T>
T diff(const std::vector<T> x, const std::vector<T> test)
{
  return diff(static_cast<const int&>(x.size()), &x[0], &test[0]);
}

//float diff(const int& N, const float* x, const float* test)
//{
//  std::vector<float> d(x, x+N);
//  const int iONE = 1;
//  const float alpha = -1.;
//  saxpy_(&N, &alpha, test, &iONE, &d[0], &iONE);
//  return snrm2_(&N, &d[0], &iONE)/snrm2_(&N, x, &iONE);
//}

#endif /* DIFF_HH */
