//=====================================================
// Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
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
#ifndef ACTION_LUDECOMP
#define ACTION_LUDECOMP

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_LUdecomp{

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_LUdecomp(const Action_LUdecomp&);

public:

    // Constructor
    Action_LUdecomp(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), A_work(size*size),
      eye_work(size*size), ipiv(size)
    {
        MESSAGE("Action_LUdecomp Constructor");

        // Make sure A is invertible
        for (int i = 0; i < size; ++i)
            A[i+i*size] += 1.2*size;
    }

    // Action name
    static std::string name()
    {
        return "LUdecomp_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*double(_size);
    }

    inline void initialize(){
        std::copy(A.begin(), A.end(), A_work.begin());
    }

    inline void calculate() {
        Interface::LUdecomp(_size, &A_work[0], &ipiv[0]);
    }

    Scalar getResidual() {
        initialize();
        calculate();

        // Initialize identity
        for (int r = 0; r < _size; ++r)
            for (int c = 0; c < _size; ++c)
                eye_work[r+_size*c] = (r == c);

        Interface::TriMatrixMatrix('u', _size, _size, &A_work[0], &eye_work[0]);

        // Hard-coded unitary diagonal
        for (int r = 0; r < _size; ++r)
            A_work[r+_size*r] = 1.;

        Interface::TriMatrixMatrix('l', _size, _size, &A_work[0], &eye_work[0]);

        Interface::axpy(_size*_size, -1., &A[0], &eye_work[0]);
        Scalar n = Interface::norm(_size*_size, &eye_work[0]);
        n /= Interface::norm(_size*_size, &A[0]);
        return n;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    vector_t A;
    vector_t A_work, eye_work;;
    std::vector<int> ipiv;

};

#endif // ACTION_LUDECOMP

