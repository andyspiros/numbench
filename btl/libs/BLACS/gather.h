#ifndef GATHER_H_
#define GATHER_H_

#define TYPENAME float
#define TYPEPREFIX s
#include "gather_impl.h"
#undef TYPENAME
#undef TYPEPREFIX

#define TYPENAME double
#define TYPEPREFIX d
#include "gather_impl.h"
#undef TYPENAME
#undef TYPEPREFIX


#endif /* GATHER_H_ */
