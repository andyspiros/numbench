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
#ifndef ACTION_TRIMATRIXMATRIX
#define ACTION_TRIMATRIXMATRIX

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_TriMatrixMatrix {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_TriMatrixMatrix(const Action_TriMatrixMatrix&);

public:

    // Constructor
    Action_TriMatrixMatrix(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), B(lc.fillVector<Scalar>(size*size)),
      B_work(size*size)
    {
        MESSAGE("Action_TriMatrixMatrix Constructor");
    }

    // Action name
    static std::string name()
    {
        return "TriMatrixMatrix_" + Interface::name();
    }

    double fpo() {
        return double(_size)*double(_size)*(double(_size) - 1);
    }

    inline void initialize(){
          std::copy(B.begin(), B.end(), B_work.begin());
    }

    inline void calculate() {
        Interface::TriMatrixMatrix('U', _size, _size, &A[0], &B_work[0]);
    }

    Scalar getResidual() {
        return 0.;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A, B;
    vector_t B_work;

};

#endif // ACTION_TRIMATRIXMATRIX
