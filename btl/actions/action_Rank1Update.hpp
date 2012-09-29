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
#ifndef ACTION_RANK1UPDATE
#define ACTION_RANK1UPDATE

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_Rank1Update {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_Rank1Update(const Action_Rank1Update&);

public:

    // Constructor
    Action_Rank1Update(int size)
    : _size(size), lc(10),
      A(lc.fillVector<Scalar>(size*size)), A_work(size*size),
      x(lc.fillVector<Scalar>(size)), y(lc.fillVector<Scalar>(size))
    {
        MESSAGE("Action_Rank1Update Constructor");
    }

    // Action name
    static std::string name()
    {
        return "Rank1Update_" + Interface::name();
    }

    double fpo() {
        return 3*double(_size)*double(_size);
    }

    inline void initialize(){
          std::copy(A.begin(), A.end(), A_work.begin());
    }

    inline void calculate() {
        Interface::Rank1Update(_size, _size, 1., &x[0], &y[0], &A_work[0]);
    }

    Scalar getResidual() {
        return 0.;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t A, x, y;
    vector_t A_work;

};

#endif // ACTION_RANK1UPDATE
