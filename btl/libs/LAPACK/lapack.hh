#ifndef LAPACK_HH
#define LAPACK_HH

#define SCALAR        float
#define SCALAR_PREFIX s
#include "lapack_.hh"
#undef SCALAR
#undef SCALAR_PREFIX

#define SCALAR        double
#define SCALAR_PREFIX d
#include "lapack_.hh"
#undef SCALAR
#undef SCALAR_PREFIX

#endif /* LAPACK_HH */
