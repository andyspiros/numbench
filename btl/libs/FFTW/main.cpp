#include "utilities.h"
#include "bench.hh"
#include "fftw_interface.hh"
#include "fftw_actions.hh"

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
  bool
  fftw_1d_forward_measure = false,
  fftw_1d_forward_estimate = false,
  fftw_1d_backward_measure = false,
  fftw_1d_backward_estimate = false,

  fftw_2d_forward_measure = false,
  fftw_2d_forward_estimate = false,
  fftw_2d_backward_measure = false,
  fftw_2d_backward_estimate = false
  ;


  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "FFTW_1D_Forward_Measure" || arg == "all") fftw_1d_forward_measure = true;
    if (arg == "FFTW_1D_Forward_Estimate" || arg == "all") fftw_1d_forward_estimate = true;
    if (arg == "FFTW_1D_Backward_Measure" || arg == "all") fftw_1d_backward_measure = true;
    if (arg == "FFTW_1D_Backward_Estimate" || arg == "all") fftw_1d_backward_estimate = true;

    if (arg == "FFTW_2D_Forward_Measure" || arg == "all") fftw_2d_forward_measure = true;
    if (arg == "FFTW_2D_Forward_Estimate" || arg == "all") fftw_2d_forward_estimate = true;
    if (arg == "FFTW_2D_Backward_Measure" || arg == "all") fftw_2d_backward_measure = true;
    if (arg == "FFTW_2D_Backward_Estimate" || arg == "all") fftw_2d_backward_estimate = true;
  }


  if (fftw_1d_forward_measure)
  bench<Action_FFTW_1D_Forward_Measure<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);
  if (fftw_1d_forward_estimate)
  bench<Action_FFTW_1D_Forward_Estimate<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);
  if (fftw_1d_backward_measure)
  bench<Action_FFTW_1D_Backward_Measure<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);
  if (fftw_1d_backward_estimate)
  bench<Action_FFTW_1D_Backward_Estimate<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);

  if (fftw_2d_forward_measure)
  bench<Action_FFTW_2D_Forward_Measure<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);
//  if (fftw_1d_forward_estimate)
//  bench<Action_FFTW_1D_Forward_Estimate<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);
//  if (fftw_1d_backward_measure)
//  bench<Action_FFTW_1D_Backward_Measure<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);
//  if (fftw_1d_backward_estimate)
//  bench<Action_FFTW_1D_Backward_Estimate<fftw_interface> >(MIN_MV,MAX_MV,NB_POINT);


  return 0;
}


