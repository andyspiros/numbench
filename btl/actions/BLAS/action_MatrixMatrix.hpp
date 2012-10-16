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
#ifndef ACTION_MATRIXMATRIX
#define ACTION_MATRIXMATRIX

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_MatrixMatrix {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_MatrixMatrix(const Action_MatrixMatrix&);

public:

    // Constructor
    Action_MatrixMatrix(int size, int seed=10)
    : _size(size), lc(seed),
      A(lc.fillVector<Scalar>(size*size)), B(lc.fillVector<Scalar>(size*size)),
      C(size*size), C_work(size*size)
    {
        MESSAGE("Action_MatrixMatrix Constructor");
    }

    // Action name
    static std::string name()
    {
        return "MatrixMatrix_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*(2*double(_size) - 1);
    }

    inline void initialize(){
          std::copy(C.begin(), C.end(), C_work.begin());
    }

    inline void calculate() {
        Interface::MatrixMatrix(false, false, _size, _size, _size,
                                1., &A[0], &B[0], 0., &C_work[0]);
    }

    Scalar getResidual() {
        return 0.;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A, B, C;
    vector_t C_work;

};

#endif // ACTION_MATRIXMATRIX
