#include "blas.h"
#include "pblas.h"
#include "blacs_interface.hh"

#define MAKE_STRING2(S) #S
#define MAKE_STRING(S) MAKE_STRING2(S)

#ifndef CAT
#define CAT2(A,B) A##B
#define CAT(A,B) CAT2(A,B)
#endif


template<class real> class pblas_interface;

/*
static char notrans = 'N';
static char trans = 'T';
static char nonunit = 'N';
static char lower = 'L';
static char right = 'R';
static char left = 'L';
static int intone = 1;
*/

#define SCALAR        float
#define SCALAR_PREFIX s
#include "pblas_interface_impl.hh"
#undef SCALAR
#undef SCALAR_PREFIX


#define SCALAR        double
#define SCALAR_PREFIX d
#include "pblas_interface_impl.hh"
#undef SCALAR
#undef SCALAR_PREFIX



