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
