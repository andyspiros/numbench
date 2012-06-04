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
#include "lapack.hh"
#include "LinearCongruential.hh"
#include "timer.hh"
#include "sizes.hh"

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

extern "C" {
  void daxpy_(const int*, const double*, const double*, const int*, double*, const int*);
  void dcopy_(const int*, const double*, const int*, double*, const int*);
  double dnrm2_(const int*, const double*, const int*);
  void dsyr_(const char*, const int*, const double*, const double*, const int*, double*, const int*);
  void dger_(const int*, const int*, const double*, const double*, const int*, const double*, const int*, double*, const int*);
  void dgemm_(const char*, const char*, const int*, const int*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);
  void dsymm_(const char*, const char*, const int*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);
  void dtrmm_(const char*, const char*, const char*, const char*, const int*, const int*, const double*, const double*, const int*, double*, const int*);
}
#include "lapack_LU.hh"
#include "lapack_cholesky.hh"
#include "lapack_SVD.hh"
#include "lapack_QR.hh"
#include "lapack_SYEV.hh"
#include "lapack_STEV.hh"

template<typename exec_t>
void test(exec_t exec, const std::string& testname, const int& max = 3000, const int& N = 100)
{
  vector<int> sizes = logsizes(1, max, N);
  Timer timer;

  ostringstream fname;
  fname << "accuracy_" << testname << "_lapack.dat";
  cout << "Testing " << testname << " --> " << fname.str() << endl;
  ofstream fs(fname.str().c_str());

  for (vector<int>::const_iterator i = sizes.begin(), end = sizes.end(); i != end; ++i) {
      const int size = *i;
      double error = 0;
      timer.start();
      int times = 0;
      do
        error += exec(size, times++);
      while (timer.elapsed() < 1. || times < 16);
      cout << " size: " << setw(4) << right << size;
      cout << setw(15) << right << error/times << "    ";
      cout << "[" << setw(6) << times << " samples]  ";
      cout << "(" << setw(3) << right << (i-sizes.begin()+1) << "/" << N << ")" << endl;
      fs << size << " " << error/times << "\n";
  }

  fs.close();
}

int main(int argc, char **argv)
{
  bool
  lu_decomp=false, cholesky = false, svd_decomp=false, qr_decomp=false,
  syev=false, stev=false
  ;

  for (int i = 1; i < argc; ++i) {
          std::string arg = argv[i];
          if (arg == "lu_decomp") lu_decomp = true;
          else if (arg == "cholesky") cholesky = true;
          else if (arg == "svd_decomp") svd_decomp = true;
          else if (arg == "qr_decomp") qr_decomp = true;
          else if (arg == "syev") syev = true;
          else if (arg == "stev") stev = true;
  }

  if (lu_decomp) {
      test(test_LU, "lu_decomp", 3000, 100);
  }

  if (cholesky) {
      test(test_cholesky, "cholesky", 3000, 100);
  }

  if (svd_decomp) {
      test(test_SVD, "svd_decomp", 1000, 90);
  }

  if (qr_decomp) {
      test(test_QR, "qr_decomp", 500, 70);
  }

  if (syev) {
      test(test_SYEV, "syev", 1000, 90);
  }

  if (stev) {
      test(test_STEV, "stev", 1000, 90);
  }

  return 0;
}
