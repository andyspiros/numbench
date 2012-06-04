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
  fftw_2d_backward_estimate = false,

  fftw_3d_forward_measure = false,
  fftw_3d_forward_estimate = false,
  fftw_3d_backward_measure = false,
  fftw_3d_backward_estimate = false
  ;
  int N = 100;


  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "FFTW_1D_Forward_Measure" || arg == "all") fftw_1d_forward_measure = true;
    else if (arg == "FFTW_1D_Forward_Estimate" || arg == "all") fftw_1d_forward_estimate = true;
    else if (arg == "FFTW_1D_Backward_Measure" || arg == "all") fftw_1d_backward_measure = true;
    else if (arg == "FFTW_1D_Backward_Estimate" || arg == "all") fftw_1d_backward_estimate = true;

    else if (arg == "FFTW_2D_Forward_Measure" || arg == "all") fftw_2d_forward_measure = true;
    else if (arg == "FFTW_2D_Forward_Estimate" || arg == "all") fftw_2d_forward_estimate = true;
    else if (arg == "FFTW_2D_Backward_Measure" || arg == "all") fftw_2d_backward_measure = true;
    else if (arg == "FFTW_2D_Backward_Estimate" || arg == "all") fftw_2d_backward_estimate = true;

    else if (arg == "FFTW_3D_Forward_Measure" || arg == "all") fftw_3d_forward_measure = true;
    else if (arg == "FFTW_3D_Forward_Estimate" || arg == "all") fftw_3d_forward_estimate = true;
    else if (arg == "FFTW_3D_Backward_Measure" || arg == "all") fftw_3d_backward_measure = true;
    else if (arg == "FFTW_3D_Backward_Estimate" || arg == "all") fftw_3d_backward_estimate = true;

    // Check switch -N
    else if (arg[0] == '-' && arg[1] == 'N') {
        if (arg[2] != '\0')
            N = atoi(arg.c_str()+2);
        else
            N = atoi(argv[++i]);
    }
  }


  if (fftw_1d_forward_measure)
  bench<Action_FFTW_1D_Forward_Measure<fftw_interface> >(MIN_MV,MAX_MV, N);
  if (fftw_1d_forward_estimate)
  bench<Action_FFTW_1D_Forward_Estimate<fftw_interface> >(MIN_MV,MAX_MV, N);
  if (fftw_1d_backward_measure)
  bench<Action_FFTW_1D_Backward_Measure<fftw_interface> >(MIN_MV,MAX_MV, N);
  if (fftw_1d_backward_estimate)
  bench<Action_FFTW_1D_Backward_Estimate<fftw_interface> >(MIN_MV,MAX_MV, N);

  if (fftw_2d_forward_measure)
  bench<Action_FFTW_2D_Forward_Measure<fftw_interface> >(MIN_MV,MAX_MV, N);
  if (fftw_2d_forward_estimate)
  bench<Action_FFTW_2D_Forward_Estimate<fftw_interface> >(MIN_MV,MAX_MV, N);
  if (fftw_2d_backward_measure)
  bench<Action_FFTW_2D_Backward_Measure<fftw_interface> >(MIN_MV,MAX_MV, N);
  if (fftw_2d_backward_estimate)
  bench<Action_FFTW_2D_Backward_Estimate<fftw_interface> >(MIN_MV,MAX_MV, N);

  if (fftw_3d_forward_measure)
  bench<Action_FFTW_3D_Forward_Measure<fftw_interface> >(MIN_MV,250, N);
  if (fftw_3d_forward_estimate)
  bench<Action_FFTW_3D_Forward_Estimate<fftw_interface> >(MIN_MV,250, N);
  if (fftw_3d_backward_measure)
  bench<Action_FFTW_3D_Backward_Measure<fftw_interface> >(MIN_MV,250, N);
  if (fftw_3d_backward_estimate)
  bench<Action_FFTW_3D_Backward_Estimate<fftw_interface> >(MIN_MV,250, N);


  return 0;
}


