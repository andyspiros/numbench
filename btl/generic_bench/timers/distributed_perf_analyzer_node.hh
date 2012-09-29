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
#ifndef _PORTABLE_PERF_ANALYZER_NODE_HH
#define _PORTABLE_PERF_ANALYZER_NODE_HH

#include "utilities.h"
#include "timers/portable_timer.hh"
#include "blacs.h"

template <class Action>
class Distributed_Perf_Analyzer_Node{
public:
  Distributed_Perf_Analyzer_Node( ):_nb_calc(0){
    MESSAGE("Distributed_Perf_Analyzer_Node Ctor");
    int temp, what = 0;
    blacs_get_(&temp, &what, &context);
  };
  Distributed_Perf_Analyzer_Node( const Distributed_Perf_Analyzer_Node& ){
    INFOS("Copy Ctor not implemented");
    exit(0);
  };
  ~Distributed_Perf_Analyzer_Node(){
    MESSAGE("Distributed_Perf_Analyzer_Node Dtor");
  };

  BTL_DONT_INLINE double eval_mflops(int size, bool silent = false)
  {
    Action action(size);

    /* Find best _nb_calc_ */
    int bcast_receive, iZERO = 0, iONE = 1;
    igebr2d_(&context, "A", " ", &iONE, &iONE, &bcast_receive, &iONE, &iZERO, &iZERO);
    while (bcast_receive > 0) {
      _nb_calc = bcast_receive;
      action.initialize();
      time_calculate(action);
      igebr2d_(&context, "A", " ", &iONE, &iONE, &bcast_receive, &iONE, &iZERO, &iZERO);
    }
    int tries = -bcast_receive;

    /* Optimize */
    for (int i = 1; i < tries; ++i) {
      Action _action(size);
      _action.initialize();
      time_calculate(_action);
    }

    /* Check */
    int do_check;
    igebr2d_(&context, "A", " ", &iONE, &iONE, &do_check, &iONE, &iZERO, &iZERO);
    if (do_check > 0) {
      action.initialize();
      action.calculate();
    }

    /* Return a void value */
    return 0.;
  }

  BTL_DONT_INLINE void time_calculate(Action & action)
  {
    // no need for time measurement
    action.calculate();
    for (int i = 0; i < _nb_calc; ++i)
      action.calculate();
  }

  unsigned long long get_nb_calc()
  {
    return _nb_calc;
  }


private:
  int context;
  unsigned long long _nb_calc;
};

#endif //_PORTABLE_PERF_ANALYZER_NODE_HH
