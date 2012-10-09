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
#ifndef ACTION_GENERALSOLVE
#define ACTION_GENERALSOLVE

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_GeneralSolve {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_GeneralSolve(const Action_GeneralSolve&);

public:

    // Constructor
    Action_GeneralSolve(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), b(lc.fillVector<Scalar>(size)),
      A_work(size*size), x_work(size), b_res(size), ipiv(size)
    {
        MESSAGE("Action_GeneralSolve Constructor");
    }

    // Action name
    static std::string name()
    {
        return "GeneralSolve_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*double(_size);
    }

    inline void initialize(){
        std::copy(A.begin(), A.end(), A_work.begin());
        std::copy(b.begin(), b.end(), x_work.begin());
    }

    inline void calculate() {
        Interface::GeneralSolve(_size, &A_work[0], &x_work[0], &ipiv[0]);
    }

    Scalar getResidual() {
        initialize();
        calculate();
        std::copy(b.begin(), b.end(), b_res.begin());
        Interface::MatrixVector(false, _size, _size, -1., &A[0], &x_work[0],
                                1., &b_res[0]);
        const Scalar Anorm = Interface::norm(_size*_size, &A[0]);
        return Interface::norm(_size, &b_res[0])/Anorm;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A, b;
    vector_t A_work, x_work, b_res;
    std::vector<int> ipiv;

};

#endif // ACTION_GENERALSOLVE
