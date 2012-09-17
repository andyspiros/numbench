#ifndef ACTIONGESV_HPP
#define ACTIONGESV_HPP

#include <vector>
#include <iostream>
#include <string>
#include "utilities/LinearCongruential.hpp"

/////////////////////////////////////
// Declaration of fortran routines //
/////////////////////////////////////

extern "C" {
    void sgesv_(const int*, const int*, float*, const int*, float*,
                float*, const int*, int*);

    void sgemv_(const char*, const int*, const int*, const float*,
                const float*, const int*, const float*, const int*,
                const float*, float*, const int*);

    float snrm2_(const int*, const float*, const int*);

    void dgesv_(const int*, const int*, double*, const int*, double*,
                double*, const int*, int*);

    void dgemv_(const char*, const int*, const int*, const double*,
                const double*, const int*, const double*, const int*,
                const double*, double*, const int*);

    double dnrm2_(const int*, const double*, const int*);
}


/*
 * Action class
 */

template<typename value_t = double>
class ActionGESV {
    typedef std::vector<value_t> storage_t;
    typedef LinearCongruential<> rangen_t;

public:
    ActionGESV(const int& size, const unsigned& seed=0)
        : size(size), rg(seed), A(rg.fillVector<value_t>(size*size, 0.)),
          Acopy(A), x(rg.fillVector<float>(size, 0.)), b(x)
    { }

    inline value_t compute();

    static std::string fileName() {
        return "accuracy_general_solve.dat";
    }

private:
    const int size;
    rangen_t rg;
    storage_t A, Acopy, x, b;
};



//////////////////////////////////////
// compute() method implementations //
//////////////////////////////////////

template<>
float ActionGESV<float>::compute()
{
    const int ONE = 1;
    const float alpha = -1., beta = 1.;
    int info;
    std::vector<float> ipiv(size);

    // Get input (b) norm
    const float bnorm = snrm2_(&size, &b[0], &ONE);

    // Perform computation
    sgesv_(&size, &ONE, &A[0], &size, &ipiv[0], &x[0], &size, &info);
    if (info != 0)
        std::cerr << "Info: " << info << "\n";

    // Compute residual
    sgemv_("N", &size, &size, &alpha, &Acopy[0], &size, &x[0], &ONE, &beta,
            &b[0], &ONE);
    return snrm2_(&size, &b[0], &ONE)/bnorm;
}


template<>
double ActionGESV<double>::compute()
{
    const int ONE = 1;
    const double alpha = -1., beta = 1.;
    int info;
    std::vector<double> ipiv(size);

    // Get input (b) norm
    const double bnorm = dnrm2_(&size, &b[0], &ONE);

    // Perform computation
    dgesv_(&size, &ONE, &A[0], &size, &ipiv[0], &x[0], &size, &info);
    if (info != 0)
        std::cerr << "Info: " << info << "\n";

    // Compute residual
    dgemv_("N", &size, &size, &alpha, &Acopy[0], &size, &x[0], &ONE, &beta,
            &b[0], &ONE);
    return dnrm2_(&size, &b[0], &ONE)/bnorm;
}

#endif //  ACTIONGESV_HPP
