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
#ifndef ACTION_ROT
#define ACTION_ROT

#include "LinearCongruential.hpp"
#include <vector>
#include <algorithm>

template<class Interface>
class Action_rot {

    typedef typename Interface::Scalar Scalar;
    typedef std::vector<Scalar> vector_t;

private:
    // Invalidate copy constructor
    Action_rot(const Action_rot&);

public:

    // Constructor
    Action_rot(int size, int seed=10)
    : _size(size), lc(seed),
      x(lc.fillVector<Scalar>(size)), y(lc.fillVector<Scalar>(size)),
      x_work(size), y_work(size)
    {
        MESSAGE("Action_rot Constructor");
    }

    // Action name
    static std::string name()
    {
        return "rot_" + Interface::name();
    }

    double fpo() {
        return 6*double(_size);
    }

    inline void initialize(){
        std::copy(x.begin(), x.end(), x_work.begin());
        std::copy(y.begin(), y.end(), y_work.begin());
    }

    inline void calculate() {
        Interface::rot(_size, &x_work[0], &y_work[0], 0.5, 0.6);
    }

    Scalar getResidual() {
        return 0.;
    }

private:
    const int _size;
    LinearCongruential<> lc;

    const vector_t x, y;
    vector_t x_work, y_work;

};

#endif // ACTION_AXPY
