#ifndef TESTACCURACY_HPP
#define TESTACCURACY_HPP

#include <utility>
#include <cmath>
#include <vector>
#include <iostream>

#include "utilities/Timer.hpp"

std::vector<int> getLogSizes(
        const int& minsize = 4,
        const int& maxsize = 3000,
        const unsigned& N = 100u
    )
{
    std::vector<int> sizes(N);
    const double amin = std::log(minsize), amax = std::log(maxsize);
    const double k = (amax-amin) / (N-1u);
    double a;

    for (unsigned i = 0; i < N; ++i) {
        a = amin + i*k;
        a = std::floor(std::exp(a)+.5);
        sizes[i] = a;
    }

    sizes.front() = minsize;
    sizes.back() = maxsize;

    return sizes;
}


template<
    template<class> class Action,
    typename value_t
>
std::pair<value_t, value_t> testAccuracy(
        const int& minsize = 4,
        const int& maxsize = 1000,
        const unsigned& N = 100u)
{
    std::vector<int> sizes = getLogSizes(minsize, maxsize, N);
    Timer t;

    for (std::vector<int>::const_iterator i = sizes.begin(), e = sizes.end();
         i != e; ++i) {

        int size = *i;
        std::cout << " -- Size: " << size << "  -  " << std::flush;

        t.start();
        int N = 0;
        double e, emean = 0., e2mean = 0.;
        do {
            Action<value_t> action(size, N);
            action.compute();
            e = action.getResidual();
            emean += e;
            e2mean += e*e;
        } while(++N < 100 && t.elapsed() < 1. || N < 4);

        std::cout << N << " samples  -  " << t.elapsed() << " seconds  -  ";

        emean /= N;
        e2mean /= N;
        e2mean = std::sqrt(e2mean - emean*emean);

        std::cout << emean << " +/- " << e2mean << std::endl;

    }
}

#endif // TESTACCURACY_HPP
