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
#ifndef ACTION_TRISOLVEMATRIX
#define ACTION_TRISOLVEMATRIX

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_TriSolveMatrix {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_TriSolveMatrix(const Action_TriSolveMatrix&);

public:

    // Constructor
    Action_TriSolveMatrix(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), B(lc.fillVector<Scalar>(size*size)),
      X_work(size*size)
    {
        MESSAGE("Action_TriSolveMatrix Constructor");

        // Adding size to the diagonal of A to ensure it is invertible
        for (int i = 0; i < size; ++i)
            A[i+size*i] += size;
    }

    // Action name
    static std::string name()
    {
        return "TriSolveMatrix_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*(double(_size) - 2);
    }

    inline void initialize(){
          std::copy(B.begin(), B.end(), X_work.begin());
    }

    inline void calculate() {
        Interface::TriSolveMatrix('U', _size, _size, &A[0], &X_work[0]);
    }

    Scalar getResidual() {
        initialize();
        calculate();
        Interface::TriMatrixMatrix('U', _size, &A[0], &X_work[0]);
        Interface::axpy(_size*_size, -1., &B[0], &X_work[0]);
        return Interface::norm(_size*_size, &X_work[0]);
    }

private:
    const int _size;
    LinearCongruential<> lc;

    vector_t A;
    const vector_t B;
    vector_t X_work;

};

#endif // ACTION_TRISOLVEMATRIX
