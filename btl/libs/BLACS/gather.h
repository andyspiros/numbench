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

template<typename T>
static void gather_matrix(std::vector<T>& GlobalMatrix, const std::vector<T>& LocalMatrix,
    const int* desc
) {
  int GlobalRows = desc[2], GlobalCols = desc[3],
      BlockRows = desc[4], BlockCols = desc[5],
      LocalRows = desc[8], LocalCols = LocalMatrix.size()/desc[8];
  int ctxt;
  {
    int ignored, what = 0;
    blacs_get_(&ignored, &what, &ctxt);
  }
  gather(ctxt, GlobalMatrix, LocalMatrix, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);
}


#endif /* GATHER_H_ */
