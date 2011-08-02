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

template<typename T>
void scatter_matrix(const std::vector<T>& GlobalMatrix, std::vector<T>& LocalMatrix,
      int *desc,
      const int& GlobalRows=0, const int& GlobalCols=0,
      const int& BlockRows=0, const int& BlockCols=0
    )
{
  int GlobalRows_ = GlobalRows, GlobalCols_ = GlobalCols,
      BlockRows_ = BlockRows, BlockCols_ = BlockCols,
      LocalRows_, LocalCols_;
  int ctxt;
  {
    int ignored, what = 0;
    blacs_get_(&ignored, &what, &ctxt);
  }
  scatter(ctxt, GlobalMatrix, LocalMatrix,
      GlobalRows_, GlobalCols_, BlockRows_, BlockCols_, LocalRows_, LocalCols_
  );

  const int iZERO = 0;
  int info;
  const int LLD = std::max(1, LocalRows_);
  descinit_(desc, &GlobalRows_, &GlobalCols_, &BlockRows_, &BlockCols_,
      &iZERO, &iZERO, &ctxt, &LLD, &info
  );
}


#endif /* SCATTER_H_ */
