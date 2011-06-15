#include <btl.hh>
#include <bench.hh>
#include <action_general_solve.hh>
#include <action_least_squares.hh>
#include <action_lu_decomp.hh>
#include <action_cholesky.hh>
#include <action_symm_ev.hh>
#include <lapack_interface.hh>

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
	bool
	general_solve=false, least_squares=false, lu_decomp=false, cholesky=false,
	symm_ev=false
	;


	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "general_solve") general_solve = true;
		else if (arg == "least_squares") least_squares = true;
		else if (arg == "lu_decomp") lu_decomp = true;
		else if (arg == "cholesky") cholesky = true;
		else if (arg == "symm_ev") symm_ev = true;
	}


	if (general_solve)
	bench<Action_general_solve<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT);

	if (least_squares)
	bench<Action_least_squares<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT);

	if (lu_decomp)
	bench<Action_lu_decomp<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT);

	if (cholesky)
	bench<Action_cholesky<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT);

	if (symm_ev)
	bench<Action_symm_ev<lapack_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT);

	return 0;
}
