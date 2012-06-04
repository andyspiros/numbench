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
