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
#ifndef ACTION_FFTW_1D
#define ACTION_FFTW_1D
#include <string>
#include <cmath>

#include "base_action_fftw.hh"


/* FORWARD - MEASURE */
template<class Interface>
class Action_FFTW_1D_Forward_Measure : public Action_FFTW_base<Interface>
{
public:
  // Constructor
  Action_FFTW_1D_Forward_Measure(const int& size) :
    Action_FFTW_base<Interface>(size, 1)
  {
    Interface::fftw_init_plan(this->p, size, this->X, this->Y, FFTW_FORWARD, FFTW_MEASURE);
  }

  // Action name
  static inline std::string name( void )
  {
    return "FFTW_1D_Forward_Measure_"+Interface::name();
  }

  // Algorithm complexity
  double nb_op_base( void ){
    const static double log_2 = std::log(2.);
    return this->size_ * std::log(this->size_)/log_2;
  }
};


/* FORWARD - ESTIMATE */
template<class Interface>
class Action_FFTW_1D_Forward_Estimate : public Action_FFTW_base<Interface>
{
public:
  // Constructor
  Action_FFTW_1D_Forward_Estimate(const int& size) :
    Action_FFTW_base<Interface>(size, 1)
  {
    Interface::fftw_init_plan(this->p, size, this->X, this->Y, FFTW_FORWARD, FFTW_ESTIMATE);
  }

  // Action name
  static inline std::string name( void )
  {
    return "FFTW_1D_Forward_Estimate_"+Interface::name();
  }

  // Algorithm complexity
  double nb_op_base( void ){
    const static double log_2 = std::log(2.);
    return this->size_ * std::log(this->size_)/log_2;
  }
};

/* BACKWARD - MEASURE */
template<class Interface>
class Action_FFTW_1D_Backward_Measure : public Action_FFTW_base<Interface>
{
public:
  // Constructor
  Action_FFTW_1D_Backward_Measure(const int& size) :
    Action_FFTW_base<Interface>(size, 1)
  {
    Interface::fftw_init_plan(this->p, size, this->X, this->Y, FFTW_BACKWARD, FFTW_MEASURE);
  }

  // Action name
  static inline std::string name( void )
  {
    return "FFTW_1D_Backward_Measure_"+Interface::name();
  }

  // Algorithm complexity
  double nb_op_base( void ){
    const static double log_2 = std::log(2.);
    return this->size_ * std::log(this->size_)/log_2;
  }
};


/* BACKWARD - ESTIMATE */
template<class Interface>
class Action_FFTW_1D_Backward_Estimate : public Action_FFTW_base<Interface>
{
public:
  // Constructor
  Action_FFTW_1D_Backward_Estimate(const int& size) :
    Action_FFTW_base<Interface>(size, 1)
  {
    Interface::fftw_init_plan(this->p, size, this->X, this->Y, FFTW_BACKWARD, FFTW_ESTIMATE);
  }

  // Action name
  static inline std::string name( void )
  {
    return "FFTW_1D_Backward_Estimate_"+Interface::name();
  }

  // Algorithm complexity
  double nb_op_base( void ){
    const static double log_2 = std::log(2.);
    return this->size_ * std::log(this->size_)/log_2;
  }
};

#endif // ACTION_FFTW_1D
