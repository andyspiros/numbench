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
#ifndef ACTION_TRISOLVEVECTOR
#define ACTION_TRISOLVEVECTOR

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_TriSolveVector {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_TriSolveVector(const Action_TriSolveVector&);

public:

    // Constructor
    Action_TriSolveVector(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), b(lc.fillVector<Scalar>(size)),
      x_work(size)
    {
        MESSAGE("Action_TriSolveVector Constructor");

        // Adding size to the diagonal of A to make it invertible
        for (int i = 0; i < size; ++i)
            A[i+size*i] += size;
    }

    // Action name
    static std::string name()
    {
        return "TriSolveVector_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size) - 2*double(_size);
    }

    inline void initialize(){
          std::copy(b.begin(), b.end(), x_work.begin());
    }

    inline void calculate() {
        Interface::TriSolveVector('U', _size, &A[0], &x_work[0]);
    }

    Scalar getResidual() {
        initialize();
        calculate();
        Interface::TriMatrixVector('U', _size, &A[0], &x_work[0]);
        Interface::axpy(_size, -1., &b[0], &x_work[0]);
        return Interface::norm(_size, &x_work[0]);
    }

//private:
    const int _size;
    LinearCongruential<> lc;

    vector_t A, b;
    vector_t x_work;

};

#endif // ACTION_TRISOLVEVECTOR
