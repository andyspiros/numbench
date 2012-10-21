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
#ifndef ACTION_MATRIXTVECTOR
#define ACTION_MATRIXTVECTOR

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_MatrixTVector {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_MatrixTVector(const Action_MatrixTVector&);

public:

    // Constructor
    Action_MatrixTVector(int size, int seed=10)
    : _size(size), lc(seed),
      A(lc.fillVector<Scalar>(size*size)), x(lc.fillVector<Scalar>(size)),
      A_work(size*size), x_work(size), y_work(size)
    {
        MESSAGE("Action_MatrixTVector Constructor");
    }

    // Action name
    static std::string name()
    {
        return "MatrixTVector_" + Interface::name();
    }

    double fpo() {
        return 2*double(_size)*double(_size) - double(_size);
    }

    inline void initialize(){
          std::copy(A.begin(), A.end(), A_work.begin());
          std::copy(x.begin(), x.end(), x_work.begin());
    }

    inline void calculate() {
        Interface::MatrixVector(true, _size, _size, 1., &A_work[0], &x_work[0],
                                0., &y_work[0]);
    }

    Scalar getResidual() {
        return 0.;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A, x;
    vector_t A_work, x_work, y_work;

};

#endif // ACTION_MATRIXVECTOR
