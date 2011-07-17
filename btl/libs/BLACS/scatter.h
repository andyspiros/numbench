#ifndef SCATTER_H_
#define SCATTER_H_

#define TYPENAME float
#define TYPEPREFIX s
#include "scatter_impl.h"
#undef TYPENAME
#undef TYPEPREFIX

#define TYPENAME double
#define TYPEPREFIX d
#include "scatter_impl.h"
#undef TYPENAME
#undef TYPEPREFIX


#endif /* SCATTER_H_ */
