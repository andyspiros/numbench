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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>

#define ADD_
extern "C" {
	void daxpy_(const int*, const double*, const double*, const int*, double*, const int*);
	void dgemv_(const char*, const int*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);
	void dtrsv_(const char*, const char*, const char*, const int*, const double*, const int*, double*, const int*);
	void dgemm_(const char*, const char*, const int*, const int*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);
	double dnrm2_(const int*, const double*, const int*);
}

using namespace std;

template<typename T>
void print_array(const int& N, const T* array) {
	for (const T *p = array, *e = array+N; p != e; ++p)
		cout << *p << " ";
}

template<typename T>
void print_matrix(const int& rows, const int& cols, const T* matrix) {
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col)
			cout << *(matrix + rows*col + row) << " ";
		cout << "\n";
	}
}

template<typename T>
vector<T> logspace(const T& min, const T& max, const unsigned& N)
{
	vector<T> result;
	result.reserve(N);

	const double emin = log(static_cast<double>(min)), emax = log(static_cast<double>(max));
	double e, r = (emax-emin)/(N-1);
	for (unsigned i = 0; i < N; ++i) {
		e = emin + i*r;
		result.push_back(static_cast<T>(exp(e)));
	}

	return result;
}

template<typename T>
vector<T> logsizes(const T& min, const T& max, const unsigned& N)
{
	if (N <= 10)
		return logspace(min, max, N);

	vector<T> result;
	result.reserve(N);

	for (unsigned i = 0; i < 9; ++i)
		result.push_back(i+1);
	vector<T> lres = logspace(10, max, N-9);
	for (unsigned i = 9; i < N; ++i)
		result.push_back(lres[i-9]);

	return result;
}

double axpy_test(const int& size)
{
	// Set up
	const int ONE = 1;
	const double alpha = 1./3.;
	double *x = new double[size], *y = new double[size];
	for (int i = 1; i <= size; ++i) {
		x[i-1] = 10. / i;
		y[i-1] = -10./(3. * i);
	}

	// Execute
	daxpy_(&size, &alpha, x, &ONE, y, &ONE);

	// Compute the error
	double error = dnrm2_(&size, y, &ONE);

	delete[] x; delete[] y;
	return error;
}

double matrix_vector_test(const int& size)
{
	// Set up
	const int ONE = 1;
	char TRANS = 'N';
	const double alpha = 1./size, beta = 1.;
	double *A = new double[size*size], *x = new double[size], *y = new double[size];
	for (int i = 1; i <= size; ++i) {
		x[i-1] = i;
		y[i-1] = -i;
		for (int j = 1; j <= size; ++j) {
			*(A+i-1+(j-1)*size) = static_cast<double>(i)/j;
		}
	}

	// Execute
	dgemv_(&TRANS, &size, &size, &alpha, A, &size, x, &ONE, &beta, y, &ONE);

	// Compute the error
	double error = dnrm2_(&size, y, &ONE);

	delete[] A; delete[] x; delete[] y;
	return error;
}

double trisolve_vector_test(const int& size)
{
	// Set up
	const int ONE = 1;
	char UPLO = 'U', TRANS = 'N', DIAG = 'U';
	double *A = new double[size*size], *x = new double[size+1], *y = new double[size];
	const double alpha = 1.;
	x[size] = 0.;
	for (int i = 1; i <= size; ++i) {
		x[size-i] = x[size-i+1] + 1./i;
		y[i-1] = -1.;
		for (int j = i; j <= size; ++j) {
			*(A+i-1+(j-1)*size) = 1. / (j-i+1);
		}
	}

	// Execute
	dtrsv_(&UPLO, &TRANS, &DIAG, &size, A, &size, x, &ONE);
	daxpy_(&size, &alpha, x, &ONE, y, &ONE);
	double error = dnrm2_(&size, y, &ONE);

	delete[] A; delete[] x; delete[] y;
	return error;
}

double matrix_matrix_test(const int& size)
{
	// rand48 initialization
	srand48(5);

	// sigma = SUM[k=1..size](k)
	double sigma = 0;
	for (int i = 1; i <= size; ++i)
		sigma += i;

	// Set up
	const int ONE = 1;
	char TRANS = 'N';
	const double alpha = drand48(), beta = -2. * alpha * sigma;
	const int size2 = size*size;
	double *A = new double[size2], *B = new double[size2], *C = new double[size2];

	for (int i = 1; i <= size; ++i)
		for (int j = 1; j <= size; ++j) {
			*(A+i-1+size*(j-1)) = static_cast<double>(j)/i;
			*(B+i-1+size*(j-1)) = j;
			*(C+i-1+size*(j-1)) = static_cast<double>(j)/(2.*i);
		}

	// Execute
	dgemm_(&TRANS, &TRANS, &size, &size, &size, &alpha, A, &size, B, &size, &beta, C, &size);
	double error = dnrm2_(&size2, C, &ONE);

	delete[] A; delete[] B; delete[] C;
	return error;
}

template<typename T>
void test(T t, const int& min, const int& max, const unsigned& N, const string& name)
{
    ostringstream fname;
	ofstream fout;
	double result;
	int N_;
	
	fname << "accuracy_" << name << "_blas.dat";
	fout.open(fname.str().c_str());
	cout << name << " test -- " << fname.str() << endl;
	vector<int> axpy_sizes = logsizes(min, max, N);
	N_ = 0;
	for (vector<int>::const_reverse_iterator i = axpy_sizes.rbegin(), e = axpy_sizes.rend(); i!=e; ++i) {
		result = t(*i);
		fout << *i << " " << result << endl;
		cout << " size: " << *i << "   " << result << "   (" << ++N_ << "/100)" << endl;
	}
	fout.close();
	cout << "\n";
}

int main(int argv, char **argc)
{
    bool
    axpy=false, axpby=false, rot=false,
    matrix_vector=false, atv=false, symv=false, syr2=false, ger=false, trisolve_vector=false,
    matrix_matrix=false, aat=false, trisolve_matrix=false, trmm=false  
    ;
  
  
    for (int i = 1; i < argv; ++i) {
        std::string arg = argc[i];
        if (arg == "axpy") axpy = true;
        else if (arg == "axpby") axpby = true;
        else if (arg == "rot") rot = true;
        else if (arg == "matrix_vector") matrix_vector = true;
        else if (arg == "atv") atv = true;
        else if (arg == "symv") symv = true;
        else if (arg == "syr2") syr2 = true;
        else if (arg == "ger") ger = true;
        else if (arg == "trisolve_vector") trisolve_vector = true;
        else if (arg == "matrix_matrix") matrix_matrix = true;
        else if (arg == "aat") aat = true;
        else if (arg == "trisolve_matrix") trisolve_matrix = true;
        else if (arg == "trmm") trmm = true;
    }
    
    
	/* AXPY test */
	if (axpy)
    test(axpy_test, 1, 1000000, 100, "axpy");
    	
    if (matrix_vector)
    test(matrix_vector_test, 1, 3000, 100, "matrix_vector");
	
	if (trisolve_vector)
    test(trisolve_vector_test, 1, 3000, 100, "trisolve_vector");
    
    if (matrix_matrix)
    test(matrix_matrix_test, 1, 2000, 100, "matrix_matrix");

    return 0;

}
