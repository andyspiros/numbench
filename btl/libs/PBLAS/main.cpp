#define DISTRIBUTED

#include "mpi.h"
#include "utilities.h"
#include "bench.hh"

#include <iostream>
//using namespace std;

#include "pblas_interface.hh"
#include "blacsinit.hh"

#include "action_parallel_axpy.hh"
#include "action_parallel_matrix_vector_product.hh"
#include "action_parallel_lu_decomp.hh"
#include "action_parallel_cholesky.hh"
#include "action_parallel_qr_decomp.hh"
#include "action_parallel_svd_decomp.hh"
#include "action_parallel_symm_ev.hh"

#include <string>

BTL_MAIN;

int main(int argc, char **argv)
{
  bool iamroot = blacsinit(&argc, &argv);

  bool
  axpy=false, matrix_vector=false,
  lu_decomp=false, cholesky=false, qr_decomp=false, svd_decomp=false,
  symm_ev=false
  ;


  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "axpy") axpy = true;
    else if (arg == "matrix_vector") matrix_vector=true;
    else if (arg == "lu_decomp") lu_decomp = true;
    else if (arg == "cholesky") cholesky = true;
    else if (arg == "qr_decomp") qr_decomp = true;
    else if (arg == "svd_decomp") svd_decomp = true;
    else if (arg == "symm_ev") symm_ev = true;
    else if(iamroot) {
      cerr << "Argument not recognized: " << arg << endl << "Exit" << endl;
      return 1;
    }
  }



  if (axpy)
  distr_bench<Action_parallel_axpy<pblas_interface<REAL_TYPE> > >(MIN_AXPY,MAX_AXPY,NB_POINT, !iamroot);

  if (matrix_vector)
  distr_bench<Action_parallel_matrix_vector_product<pblas_interface<REAL_TYPE> > >(MIN_MV,MAX_MV,NB_POINT, !iamroot);

  if (lu_decomp)
  distr_bench<Action_parallel_lu_decomp<pblas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT, !iamroot);

  if (cholesky)
  distr_bench<Action_parallel_cholesky<pblas_interface<REAL_TYPE> > >(MIN_MM, MAX_MM, NB_POINT, !iamroot);

  if (qr_decomp)
  distr_bench<Action_parallel_qr_decomp<pblas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT, !iamroot);

  if (svd_decomp)
  distr_bench<Action_parallel_svd_decomp<pblas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT, !iamroot);

  if (symm_ev)
  distr_bench<Action_parallel_symm_ev<pblas_interface<REAL_TYPE> > >(MIN_MM,MAX_MM,NB_POINT, !iamroot);


  int iZERO = 0;
  blacs_exit_(&iZERO);
  return 0;
}


