#include "mpi.h"
#include "utilities.h"
#include "bench.hh"

#include <iostream>
//using namespace std;

#include "pblas_interface.hh"
#include "action_parallel_matrix_vector_product.hh"

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  int myid, numproc, context, procrows = 2, proccols = 2, iZERO = 0;
  blacs_pinfo_(&myid, &numproc);
  blacs_get_(&iZERO, &iZERO, &context);
  blacs_gridinit_(&context, "Row-major", &procrows, &proccols);
  bool iamroot = (myid == 0);

  distr_bench<Action_parallel_matrix_vector_product<pblas_interface<REAL_TYPE> > >(10,MAX_MV,NB_POINT,!iamroot);

//  blacs_exit_(&iZERO);
  MPI_Finalize();
  return 0;
}


