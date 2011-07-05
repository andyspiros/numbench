#include "utilities.h"
#include "bench.hh"
#include "fftw_interface.hh"
#include "fftw_actions.hh"

#include <string>

BTL_MAIN;

int main(int argv, char **argc)
{
  bool
  fftw_1d_forward_measure = false,
  fftw_1d_forward_estimate = false,
  fftw_1d_backward_measure = false,
  fftw_1d_backward_estimate = false
  ;


  for (int i = 1; i < argv; ++i) {
    std::string arg = argc[i];
    if (arg == "fftw_1d_forward_measure" || arg == "all") fftw_1d_forward_measure = true;
    if (arg == "fftw_1d_forward_estimate" || arg == "all") fftw_1d_forward_estimate = true;
    if (arg == "fftw_1d_backward_measure" || arg == "all") fftw_1d_backward_measure = true;
    if (arg == "fftw_1d_backward_estimate" || arg == "all") fftw_1d_backward_estimate = true;
  }


  if (fftw_1d_forward_measure)
  bench<Action_FFTW_1D_Forward_Measure<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);

  if (fftw_1d_forward_estimate)
  bench<Action_FFTW_1D_Forward_Estimate<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);

  if (fftw_1d_backward_measure)
  bench<Action_FFTW_1D_Backward_Measure<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);

  if (fftw_1d_backward_estimate)
  bench<Action_FFTW_1D_Backward_Estimate<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);


  return 0;
}


