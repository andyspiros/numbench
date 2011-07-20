#ifndef BTL_FFTW_INTERFACE_H
#define BTL_FFTW_INTERFACE_H

#include <complex>
#include <fftw3.h>
#include <vector>

class fftw_interface
{
public:

	static inline std::string name()
	{
		return std::string("fftw");
	}

	typedef std::complex<double> real_type;
	typedef std::vector<std::complex<double> > stl_vector;

	typedef fftw_complex* gene_vector;

	typedef fftw_plan plan;

	static inline void free_vector(gene_vector & B){
		fftw_free(B);
	}

	static inline void vector_from_stl(gene_vector & B, stl_vector & B_stl){
		int N = B_stl.size();
		B = (gene_vector) fftw_malloc(sizeof(fftw_complex) * N);
		std::complex<double>* B_cplx = reinterpret_cast<std::complex<double>* >(B);

		for (int i=0;i<N;i++)
			B_cplx[i] = B_stl[i];
	}

	static inline void vector_to_stl(gene_vector & B, stl_vector & B_stl){
		int N = B_stl.size();
		std::complex<double>* B_cplx = reinterpret_cast<std::complex<double>* >(B);
		for (int i=0;i<N;i++)
			B_stl[i] = B_cplx[i];
	}

	static inline void copy_vector(const gene_vector & source, gene_vector & cible, int N){
		for (int i=0;i<N;i++) {
			cible[i][0]=source[i][0];
			cible[i][1]=source[i][1];
		}
	}

	/* Begin FFTW operations */

	static inline void fftw_init_plan(plan & p, const int & N, gene_vector & x, gene_vector & y, const int & sign, const int & flags){
		p = fftw_plan_dft_1d(N, x, y, sign, flags);
	}

	static inline void fftw_init_plan_2d(plan & p, const int & N, gene_vector & x, gene_vector& y, const int & sign, const int & flags){
		p = fftw_plan_dft_2d(N, N, x, y, sign, flags);
	}

	static inline void fftw_run(plan & p){
		fftw_execute(p);
	}
};

#endif // BTL_FFTW_INTERFACE_H
