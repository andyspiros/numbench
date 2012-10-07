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
#ifndef ACTION_MATRIXVECTOR
#define ACTION_MATRIXVECTOR

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_MatrixVector {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_MatrixVector(const Action_MatrixVector&);

public:

    // Constructor
    Action_MatrixVector(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), x(lc.fillVector<Scalar>(size)),
      y(size), y_work(size)
    {
        MESSAGE("Action_MatrixVector Constructor");
    }

    // Action name
    static std::string name()
    {
        return "MatrixVector_" + Interface::name();
    }

    double fpo() {
        return 2*double(_size)*double(_size) - double(_size);
    }

    inline void initialize(){
          std::copy(y.begin(), y.end(), y_work.begin());
    }

    inline void calculate() {
        Interface::MatrixVector(false, _size, _size, 1., &A[0], &x[0],
                                0., &y_work[0]);
    }

    Scalar getResidual() {
        return 0.;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A, x, y;
    vector_t y_work;

};

#endif // ACTION_MATRIXVECTOR
