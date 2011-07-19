#ifndef _PORTABLE_PERF_ANALYZER_ROOT_HH
#define _PORTABLE_PERF_ANALYZER_ROOT_HH

#include "utilities.h"
#include "timers/portable_timer.hh"
#include "blacs.h"

template <class Action>
class Distributed_Perf_Analyzer_Root{
public:
  Distributed_Perf_Analyzer_Root( ):_nb_calc(0), m_time_action(0), _chronos(){
    MESSAGE("Distributed_Perf_Analyzer_Root Ctor");
    int temp, what = 0;
    blacs_get_(&temp, &what, &context);
  };
  Distributed_Perf_Analyzer_Root( const Distributed_Perf_Analyzer_Root & ){
    INFOS("Copy Ctor not implemented");
    exit(0);
  };
  ~Distributed_Perf_Analyzer_Root(){
    MESSAGE("Distributed_Perf_Analyzer_Root Dtor");
  };

  BTL_DONT_INLINE double eval_mflops(int size, bool silent = false)
  {
    Action action(size);
    m_time_action = 0;
    _nb_calc = 0;

    /* Find best _nb_calc_ */
    int bcast_send = _nb_calc;
    int iONE = 1;
    while (m_time_action < MIN_TIME) {
      _nb_calc = _nb_calc ? 2*_nb_calc : 1;
      bcast_send = _nb_calc;
      igebs2d_(&context, "A", " ", &iONE, &iONE, &bcast_send, &iONE);
      action.initialize();
      m_time_action = time_calculate(action);
    }
    int tries = BtlConfig::Instance.tries;
    bcast_send = -tries;
    igebs2d_(&context, "A", " ", &iONE, &iONE, &bcast_send, &iONE);

    /* Optimize */
    for (int i = 1; i < tries; ++i) {
      Action _action(size);
      if (!silent)
        std::cout << " " << _action.nb_op_base()*_nb_calc/(m_time_action*1e6) << " ";
      _action.initialize();
      m_time_action = std::min(m_time_action, time_calculate(_action));
    }
    double time_action = m_time_action / (double(_nb_calc));

    /* Check */
    int do_check = (BtlConfig::Instance.checkResults && size<128) ? 1 : 1;
    igebs2d_(&context, "A", " ", &iONE, &iONE, &do_check, &iONE);
    if (do_check > 0) {
      action.initialize();
      action.calculate();
      action.check_result();
    }

    return action.nb_op_base()/(time_action*1e6);
  }

  BTL_DONT_INLINE double time_calculate(Action & action)
  {
    // time measurement
    action.calculate();
    _chronos.start();
    for (int ii=0; ii<_nb_calc; ii++)
    {
      action.calculate();
    }
    _chronos.stop();
    return _chronos.user_time();
  }

  unsigned long long get_nb_calc()
  {
    return _nb_calc;
  }


private:
  int context;
  unsigned long long _nb_calc;
  double m_time_action;
  Portable_Timer _chronos;

};

#endif //_PORTABLE_PERF_ANALYZER_ROOT_HH

