#ifndef ACTIONGESV_HPP
#define ACTIONGESV_HPP

#include <vector>
#include <iostream>
#include "LinearCongruential.hpp"

extern "C" {
    void dgesv_(const int*, const int*, double*, const int*, double*,
                double*, const int*, int*);

    void dgemv_(const char*, const int*, const int*, const double*,
                const double*, const int*, const double*, const int*,
                const double*, double*, const int*);

    double dnrm2_(const int*, const double*, const int*);
}

template<typename value_t = double>
class ActionGESV {
    typedef std::vector<value_t> storage_t;
    typedef LinearCongruential<> rangen_t;

public:
    ActionGESV(const int& size, const unsigned& seed=0)
        : size(size), rg(seed), A(rg.fillVector<>(size*size, 0.)), Acopy(A),
          x(rg.fillVector(size, 0.)), b(x), bcopy(b)
    { }

    void compute() {
        const int ONE = 1;
        int info;
        std::vector<value_t> ipiv(size);
        dgesv_(&size, &ONE, &A[0], &size, &ipiv[0], &x[0], &size, &info);
        if (info != 0)
            std::cerr << "Info: " << info << "\n";
    }

    double getResidual() {
        const double alpha = -1., beta = 1.;
        const int ONE = 1;
        dgemv_("N", &size, &size, &alpha, &Acopy[0], &size, &x[0], &ONE, &beta,
                &b[0], &ONE);
        return dnrm2_(&size, &b[0], &ONE)/dnrm2_(&size, &bcopy[0], &ONE);
    }

//private:
    const int size;
    rangen_t rg;
    storage_t A, Acopy, x, b, bcopy;
};

#endif //  ACTIONGESV_HPP
