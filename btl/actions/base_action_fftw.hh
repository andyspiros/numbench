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
#ifndef BASE_ACTION_FFTW_HH
#define BASE_ACTION_FFTW_HH

#include "utilities.h"
#include "init/init_function.hh"
#include "init/init_vector.hh"

inline int power(const int& base, const int& exp) {
  int ret = 1;
  for (int i = 0; i < exp; ++i)
    ret *= base;
  return ret;
}

template<class Interface>
class Action_FFTW_base
{
public:
  // Constructor
  Action_FFTW_base(const int& size, const int& dimensions) :
      size_(size), dimensions_(dimensions), N_(power(size, dimensions))
  {
    // STL vector initialization
    init_vector < pseudo_random > (X_stl, N_);
    init_vector < null_function > (Y_stl, N_);

    // Generic vector initialization
    Interface::vector_from_stl(X, X_stl);
    Interface::vector_from_stl(Y, Y_stl);

    // To be done by the child:
    /* initialize plan! */
  }

  // Invalidate copy constructor
  Action_FFTW_base( const Action_FFTW_base & )
  {
    INFOS("illegal call to Action_FFTW_base Copy Ctor");
    exit(1);
  }

  // Destructor:
  // frees the memory
  ~Action_FFTW_base()
  {
    Interface::free_vector(X);
    Interface::free_vector(Y);
  }

  inline void initialize() { }

  inline void calculate( void ) {
    Interface::fftw_run(p);
  }

  void check_result( void ){
  }


protected:
  const int size_, dimensions_, N_;

  typename Interface::stl_vector X_stl, Y_stl;
  typename Interface::gene_vector X, Y;

  typename Interface::plan p;
};

#endif /* BASE_ACTION_FFTW_HH */
