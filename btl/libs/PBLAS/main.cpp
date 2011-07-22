#define DISTRIBUTED

#include "mpi.h"
#include "utilities.h"
#include "bench.hh"

#include <iostream>
//using namespace std;

#include "pblas_interface.hh"
#include "blacsinit.hh"
#include "action_parallel_matrix_vector_product.hh"
#include "action_parallel_lu_decomp.hh"
#include "action_parallel_axpy.hh"

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
  bool iamroot = blacsinit(&argc, &argv);

//  distr_bench<Action_parallel_matrix_vector_product<pblas_interface<double> > >(10,MAX_MV,NB_POINT,!iamroot);
//  distr_bench<Action_parallel_axpy<pblas_interface<REAL_TYPE> > >(10,MAX_MV,NB_POINT,!iamroot);
  distr_bench<Action_parallel_lu_decomp<pblas_interface<REAL_TYPE> > >(10,MAX_MV,NB_POINT,!iamroot);
//  Action_parallel_lu_decomp<pblas_interface<REAL_TYPE> > action(8);
//  action.initialize();
//  action.calculate();
//  action.check_result();

  int iZERO = 0;
  blacs_exit_(&iZERO);
  return 0;
}


