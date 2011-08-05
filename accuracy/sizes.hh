#ifndef SIZES_HH
#define SIZES_HH

#include <vector>
#include <cmath>

template<typename T>
std::vector<T> logspace(const T& min, const T& max, const unsigned& N)
{
        std::vector<T> result;
        result.reserve(N);

        const double emin = std::log(static_cast<double>(min)),
                     emax = std::log(static_cast<double>(max));
        double e, r = (emax-emin)/(N-1);
        for (unsigned i = 0; i < N; ++i) {
                e = emin + i*r;
                result.push_back(static_cast<T>(exp(e)));
        }

        return result;
}

template<typename T>
std::vector<T> logsizes(const T& min, const T& max, const unsigned& N)
{
        if (N <= 10)
                return logspace(min, max, N);

        std::vector<T> result;
        result.reserve(N);

        for (unsigned i = 0; i < 9; ++i)
                result.push_back(i+1);
        std::vector<T> lres = logspace(10, max, N-9);
        for (unsigned i = 9; i < N; ++i)
                result.push_back(lres[i-9]);

        return result;
}

#endif /* SIZES_HH */
