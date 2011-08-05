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
