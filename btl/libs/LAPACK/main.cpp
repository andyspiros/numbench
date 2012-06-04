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
#include <btl.hh>
#include <bench.hh>
#include <action_general_solve.hh>
#include <action_least_squares.hh>
#include <action_lu_decomp.hh>
#include <action_cholesky.hh>
#include <action_qr_decomp.hh>
#include <action_svd_decomp.hh>
#include <action_syev.hh>
#include <action_stev.hh>
#include <action_symm_ev.hh>
#include <lapack_interface.hh>

#include <string>
#include <cstdlib>

BTL_MAIN;

int main(int argc, char **argv)
{
	bool
	general_solve=false, least_squares=false, lu_decomp=false, cholesky=false, qr_decomp=false, svd_decomp=false,
	syev=false, stev=false, symm_ev=false
	;
	int N = 100;


	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "general_solve") general_solve = true;
		else if (arg == "least_squares") least_squares = true;
		else if (arg == "lu_decomp") lu_decomp = true;
		else if (arg == "cholesky") cholesky = true;
		else if (arg == "qr_decomp") qr_decomp = true;
		else if (arg == "svd_decomp") svd_decomp = true;
		else if (arg == "syev") syev = true;
		else if (arg == "stev") stev = true;
		else if (arg == "symm_ev") symm_ev = true;

		// Check switch -N
		else if (arg[0] == '-' && arg[1] == 'N') {
		    if (arg[2] != '\0')
		        N = atoi(arg.c_str()+2);
		    else
		        N = atoi(argv[++i]);
		}
	}


	if (general_solve)
	bench<Action_general_solve<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,N);

	if (least_squares)
	bench<Action_least_squares<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,N);

	if (lu_decomp)
	bench<Action_lu_decomp<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,N);

	if (cholesky)
	bench<Action_cholesky<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,N);

	if (qr_decomp)
	bench<Action_qr_decomp<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,N);

	if (svd_decomp)
	bench<Action_svd_decomp<lapack_interface<REAL_TYPE> > >(MIN_MM,750,N);

	if (syev)
	bench<Action_syev<lapack_interface<REAL_TYPE> > >(MIN_MM,750,N);

	if (stev)
	bench<Action_stev<lapack_interface<REAL_TYPE> > >(MIN_MM,1000,N);

	if (symm_ev)
	bench<Action_symm_ev<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,N);

	return 0;
}
