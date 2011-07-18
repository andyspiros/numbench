//=====================================================
// File   :  bench.hh
// Author :  L. Plagne <laurent.plagne@edf.fr)>
// Copyright (C) EDF R&D,  lun sep 30 14:23:16 CEST 2002
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
#ifndef BENCH_HH
#define BENCH_HH

#include "btl.hh"
#include "bench_parameter.hh"
#include <iostream>
#include "utilities.h"
#include "size_lin_log.hh"
#include "xy_file.hh"
#include <vector>
#include <string>
#include "timers/portable_perf_analyzer.hh"
#include "timers/distributed_perf_analyzer_root.hh"
#include "timers/distributed_perf_analyzer_node.hh"
// #include "timers/mixed_perf_analyzer.hh"
// #include "timers/x86_perf_analyzer.hh"
// #include "timers/STL_perf_analyzer.hh"
#ifdef HAVE_MKL
extern "C" void cblas_saxpy(const int, const float, const float*, const int, float *, const int);
#endif

template <template<class> class Perf_Analyzer, class Action>
BTL_DONT_INLINE void bench( int size_min, int size_max, int nb_point, bool silent = false )
{
  std::string filename = "bench_"+Action::name()+".dat";

  if (!silent) { INFOS("starting " <<filename); }

  // utilities

  std::vector<double> tab_mflops(nb_point);
  std::vector<int> tab_sizes(nb_point);

  // matrices and vector size calculations
  size_lin_log(nb_point,size_min,size_max,tab_sizes);

  // loop on matrix size
  Perf_Analyzer<Action> perf_action;
  for (int i=nb_point-1; i>=0; i--)
  {
    if (!silent)
      std::cout << " " << "size = " << tab_sizes[i] << "  " << std::flush;

    BTL_DISABLE_SSE_EXCEPTIONS();

    tab_mflops[i] = perf_action.eval_mflops(tab_sizes[i], silent);
    if (!silent)
      std::cout << tab_mflops[i] << " MFlops    (" << nb_point-i << "/" << nb_point << ")" << std::endl;
  }

  // dump the result in a file  :
  if (!silent) dump_xy_file(tab_sizes, tab_mflops, filename);

}

// default Perf Analyzer

template <class Action>
BTL_DONT_INLINE void bench( int size_min, int size_max, int nb_point, bool silent = false)
{
  bench<Portable_Perf_Analyzer,Action>(size_min,size_max,nb_point,silent);
}

// distributed Perf Analyzer

template <class Action>
BTL_DONT_INLINE void distr_bench( int size_min, int size_max, int nb_point, bool silent = false)
{
  int myid, nproc;
  blacs_pinfo_(&myid, &nproc);
  if (myid)
    bench<Distributed_Perf_Analyzer_Node, Action>(size_min, size_max, nb_point, silent);
  else
    bench<Distributed_Perf_Analyzer_Root, Action>(size_min, size_max, nb_point, silent);
}

#endif
