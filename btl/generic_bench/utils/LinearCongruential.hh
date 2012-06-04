//=====================================================
// Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
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
#ifndef LINEARCONGRUENTIAL_HH_
#define LINEARCONGRUENTIAL_HH_

#include <vector>

class LinearCongruential
{
  typedef std::vector<unsigned> buffer_t;
  typedef unsigned int_t;

public:
  LinearCongruential(const int_t& seed) :
    a_(1664525u), c_(1013904223u), m_(getM()), i_(0)
  {
    buffer_.resize(4096/sizeof(unsigned));
    fillBuffer(seed);
  }

  int_t a() const { return a_; }
  int_t c() const { return c_; }
  int_t m() const { return m_; }

  int_t get_int() {
    if (i_ >= buffer_.size()) {
        fillBuffer();
        i_ = 0;
    }
    return buffer_.at(i_++);
  }

  double get_01() {
    return static_cast<double>(get_int())/static_cast<double>(m_);
  }

private:
  buffer_t buffer_;
  const int_t a_, c_, m_;
  std::size_t i_;

  void fillBuffer(const int_t& seed)
  {
    buffer_.front() = (seed*a_+c_) & m_;
    for (
        typename buffer_t::iterator i = buffer_.begin()+1, end = buffer_.end();
        i != end; ++i)
      *i = (*(i-1)*a_ + c_) & m_;
  }

  void fillBuffer()
  {
    const int_t seed = buffer_.back();
    fillBuffer(seed);
  }

  static int_t getM()
  {
    int_t _m = 1;
    for (int i = 1; i < 32; ++i) {
      _m <<= 1;
      _m += 1;
    }
    return _m;
  }
};

#endif /* LINEARCONGRUENTIAL_HH_ */
