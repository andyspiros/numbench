#include "mpi.h"
#include "utilities.h"
#include "bench.hh"

#include <iostream>
//using namespace std;

#include "blacsinit.hh"
#include "pblas_interface.hh"
#include "action_parallel_matrix_vector_product.hh"

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
  bool iamroot = blacsinit(&argc, &argv);

  distr_bench<Action_parallel_matrix_vector_product<pblas_interface<double> > >(10,MAX_MV,NB_POINT,!iamroot);
//  Action_parallel_matrix_vector_product<pblas_interface<double> > action(3000);
//  action.initialize();
//  action.calculate();
//  action.check_result();

  int iZERO = 0;
  blacs_exit_(&iZERO);
  return 0;
}


