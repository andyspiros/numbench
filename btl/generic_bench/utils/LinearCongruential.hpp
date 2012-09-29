#ifndef LINEARCONGRUENTIAL_HPP
#define LINEARCONGRUENTIAL_HPP

#include "stdint.h"
#include <vector>
#include <limits>

template<
    typename value_t = uint32_t,
    value_t a = 1664525u,
    value_t c = 1013904223u
>
class LinearCongruential {
    typedef typename std::vector<value_t> buf_t;
    typedef typename buf_t::iterator iterator_t;
    typedef typename buf_t::const_iterator constiterator_t;

public:
    LinearCongruential(const value_t& seed, const int bufsize = 4*1024)
        : maxvalue(std::numeric_limits<value_t>::max())
    {
        buffer.resize(bufsize);
        fillBuffer(seed);
    }

    value_t operator()()
    {
        if (j == buffer.end())
            fillBuffer(buffer.back());
        return *(j++);
    }

    template<typename result_t>
    void fillVector (
            result_t *vector,
            const int& size,
            const result_t& min = 0.,
            const result_t& max = 1.
        )
    {
        const result_t maxvalue = this->maxvalue;
        const result_t diff = max-min;

        for (const result_t *const endp = vector+size; vector!=endp; ++vector)
            *vector = diff * (this->operator()() / maxvalue) + min;
    }


    template<typename result_t>
    std::vector<result_t> fillVector (
            const int& size,
            const result_t& min = 0.,
            const result_t& max = 1.
        )
    {
        std::vector<result_t> result(size);
        fillVector(&result[0], size, min, max);
        return result;
    }

private:
    buf_t buffer;
    constiterator_t j;

    const value_t maxvalue;

    void fillBuffer(int seed)
    {
        for (iterator_t i = buffer.begin(), e = buffer.end(); i!=e; ++i) {
            *i = ((seed *= a) += c);
        }

        j = buffer.begin();
    }
};

#endif // LINEARCONGRUENTIAL_HPP
