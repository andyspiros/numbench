#include <btl.hh>
#include <bench.hh>
#include <action_general_solve.hh>
#include <action_least_squares.hh>
#include <lapack_interface.hh>

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
	bool
	general_solve=false, least_squares=false
	;


	for (int i = 1; i < argv; ++i) {
		std::string arg = argc[i];
		if (arg == "general_solve") general_solve = true;
		else if (arg == "least_squares") least_squares = true;
	}


	if (general_solve)
	bench<Action_general_solve<lapack_interface<REAL_TYPE> > >(0,2000,NB_POINT);

	if (least_squares)
	bench<Action_least_squares<lapack_interface<REAL_TYPE> > >(0,2000,NB_POINT);

	return 0;
}
