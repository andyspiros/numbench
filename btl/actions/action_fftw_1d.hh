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
