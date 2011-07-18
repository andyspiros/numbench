#ifndef BLACSINIT_HH
#define BLACSINIT_HH

#include <cmath>

bool blacsinit(int *argc, char ***argv)
{
  int context, myid, numproc, prows, pcols, iZERO = 0;
  MPI_Init(argc, argv);
  blacs_pinfo_(&myid, &numproc);
  blacs_get_(&iZERO, &iZERO, &context);

  int p = static_cast<double>(std::sqrt(static_cast<double>(numproc)) + 1.);
  while (numproc % p) --p;
  prows = p; pcols = numproc/p;

  blacs_gridinit_(&context, "Row-major", &prows, &pcols);
  return (myid == 0);
}

#endif /* BLACSINIT_HH */
