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
#ifndef ACTION_MATRIXTMATRIX
#define ACTION_MATRIXTMATRIX

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_MatrixTMatrix {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_MatrixTMatrix(const Action_MatrixTMatrix&);

public:

    // Constructor
    Action_MatrixTMatrix(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), B(lc.fillVector<Scalar>(size*size)),
      C(size*size), C_work(size*size)
    {
        MESSAGE("Action_MatrixTMatrix Constructor");
    }

    // Action name
    static std::string name()
    {
        return "MatrixTMatrix_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*(2*double(_size) - 1);
    }

    inline void initialize(){
          std::copy(C.begin(), C.end(), C_work.begin());
    }

    inline void calculate() {
        Interface::MatrixMatrix(true, false, _size, _size, _size,
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

#endif // ACTION_MATRIXTMATRIX
