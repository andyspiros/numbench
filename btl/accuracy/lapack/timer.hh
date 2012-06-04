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
#ifndef TIMER_HH
#define TIMER_HH

#include <sys/time.h>

class Timer
{
public:
  Timer() : ZERO(0.)
  { }

  void start()
  {
    t = walltime(&ZERO);
  }

  double elapsed()
  {
    return walltime(&t);
  }

private:
  double t;

  const double ZERO;

  static double walltime(const double *t0)
  {
    double mic;
    double time;
    double mega = 0.000001;
    struct timeval tp;
    struct timezone tzp;
    static long base_sec = 0;
    static long base_usec = 0;

    (void) gettimeofday(&tp, &tzp);
    if (base_sec == 0) {
      base_sec = tp.tv_sec;
      base_usec = tp.tv_usec;
    }

    time = (double) (tp.tv_sec - base_sec);
    mic = (double) (tp.tv_usec - base_usec);
    time = (time + mic * mega) - *t0;
    return (time);
  }
};

#endif /* TIMER_HH */
