#ifndef TESTACCURACY_HPP
#define TESTACCURACY_HPP

#include <utility>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>

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

template<typename value_t> void writeToFile(
        const std::vector<int>& sizes,
        const std::vector<value_t>& means,
        const std::vector<value_t>& stds,
        const std::string& filename
    )
{
    std::ofstream fs(filename.c_str());
    fs.precision(10);
    const int N = sizes.size();

    for(int i = 0; i < N; ++i)
        fs << sizes[i] << " " << means[i] << " " << stds[i] << "\n";

    fs.close();
}


template< template<class> class Action, typename value_t >
void testAccuracy(
        const int& minsize = 4,
        const int& maxsize = 1000,
        const unsigned& N = 100u
    )
{
    const std::string filename = Action<value_t>::fileName();
    std::cout << "Beginning operation on file " << filename << std::endl;

    const std::vector<int> sizes = getLogSizes(minsize, maxsize, N);
    std::vector<value_t> means(N), stds(N);

    Timer t;

    for (int i = 0; i < N; ++i) {

        int size = sizes[i];
        std::printf(" -- Size %6i  -  ", size);
        std::cout << std::flush;

        int N = 0;
        double e, emean = 0., e2mean = 0.;

        t.start();
        do {
            Action<value_t> action(size, N);
            action.compute();
            e = action.getResidual();
            emean += e;
            e2mean += e*e;
        } while(++N < 100 && t.elapsed() < 1. || N < 4);

        std::printf("%3i samples  -  %6f seconds  -  ", N, t.elapsed());

        emean /= N;
        e2mean /= N;
        e2mean = std::sqrt(e2mean - emean*emean);

        std::printf("%3e +/- %3e", emean, e2mean);
        std::cout << std::endl;
        means[i] = emean;
        stds[i] = e2mean;
    }

    std::cout << "Writing results to file " << filename << "\n" << std::endl;
    writeToFile(sizes, means, stds, filename);
}

#endif // TESTACCURACY_HPP
