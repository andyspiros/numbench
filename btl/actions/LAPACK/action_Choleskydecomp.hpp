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
#ifndef ACTION_CHOLESKYDECOMP
#define ACTION_CHOLESKYDECOMP

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_Choleskydecomp{

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_Choleskydecomp(const Action_Choleskydecomp&);

public:

    // Constructor
    Action_Choleskydecomp(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), A_work(size*size)
    {
        MESSAGE("Action_Choleskydecomp Constructor");

        // Make A positive definite
        for (int i = 0; i < size; ++i)
            A[i+i*size] += 1.2*size;
    }

    // Action name
    static std::string name()
    {
        return "Choleskydecomp_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*double(_size);
    }

    inline void initialize(){
        std::copy(A.begin(), A.end(), A_work.begin());
    }

    inline void calculate() {
        Interface::Choleskydecomp('U', _size, &A_work[0]);
    }

    Scalar getResidual() {
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A;
    vector_t A_work, eye_work;;
    std::vector<int> ipiv;

};

#endif // ACTION_CHOLESKYDECOMP

