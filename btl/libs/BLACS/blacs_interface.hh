#ifndef BTL_BLACS_INTERFACE_H
#define BTL_BLACS_INTERFACE_H

#include <vector>
#include <algorithm>
#include "blacs.h"
extern "C" {
  void descinit_(int*, const int*, const int*, const int*, const int*, const int*, const int*, const int*, const int*, int*);
  int numroc_(const int*, const int*, const int*, const int*, const int*);
}

#include "scatter.h"
#include "gather.h"

template<typename real>
class blacs_interface
{

public:
  typedef real real_type;
  typedef std::vector<real_type> stl_vector;
  typedef stl_vector stl_matrix;

  typedef real* gene_matrix;
  typedef real* gene_vector;


  static void free_matrix(gene_matrix & A, int N){
    delete A;
  }

  static void free_vector(gene_vector & B){
    delete B;
  }

  static inline void matrix_from_stl(gene_matrix & A, stl_matrix & A_stl){
    int N = A_stl.size();
    A = new real[N];
    for (int j=0;j<N;j++)
        A[j] = A_stl[j];
  }

  static inline void vector_from_stl(gene_vector & B, stl_vector & B_stl){
    int N = B_stl.size();
    B = new real[N];
    for (int i=0;i<N;i++)
      B[i] = B_stl[i];
  }

  static inline void vector_to_stl(gene_vector & B, stl_vector & B_stl){
    int N = B_stl.size();
    for (int i=0;i<N;i++)
      B_stl[i] = B[i];
  }

  static inline void matrix_to_stl(gene_matrix & A, stl_matrix & A_stl){
    int N = A_stl.size();
    for (int i=0;i<N;i++)
      A_stl[i] = A[i];
  }

  static inline void copy_vector(const gene_vector & source, gene_vector & cible, int N){
    for (int i=0;i<N;i++)
      cible[i]=source[i];
  }

  static inline void copy_matrix(const gene_matrix & source, gene_matrix & cible, int N){
    for (int i=0;i<N;i++)
      cible[i]=source[i];
  }


public:
  static int context() {
    int ctxt, ignored, what = 0;
    blacs_get_(&ignored, &what, &ctxt);
    return ctxt;
  }


  static void scatter_matrix(const stl_vector& GlobalMatrix, stl_vector& LocalMatrix,
      int& GlobalRows, int& GlobalCols,
      int&  BlockRows, int&  BlockCols,
      int&  LocalRows, int&  LocalCols
  ) {
    scatter(context(), GlobalMatrix, LocalMatrix, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);
  }

  static void scatter_matrix(const stl_vector& GlobalMatrix, stl_vector& LocalMatrix,
      int *desc,
      const int& GlobalRows=0, const int& GlobalCols=0,
      const int& BlockRows=0, const int& BlockCols=0
  ) {
    int GlobalRows_ = GlobalRows, GlobalCols_ = GlobalCols,
        BlockRows_ = BlockRows, BlockCols_ = BlockCols,
        LocalRows_, LocalCols_;
    const int ctxt = context();
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

  static void gather_matrix(stl_vector& GlobalMatrix, const stl_vector& LocalMatrix,
      int& GlobalRows, int& GlobalCols,
      int&  BlockRows, int&  BlockCols,
      int&  LocalRows, int&  LocalCols
  ) {
    gather(context(), GlobalMatrix, LocalMatrix, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);
  }

  static void gather_matrix(stl_vector& GlobalMatrix, const stl_vector& LocalMatrix,
      int* desc
  ) {
    int GlobalRows = desc[2], GlobalCols = desc[3],
        BlockRows = desc[4], BlockCols = desc[5],
        LocalRows = desc[8], LocalCols = LocalMatrix.size()/desc[8];
    const int ctxt = context();
    gather(ctxt, GlobalMatrix, LocalMatrix, GlobalRows, GlobalCols, BlockRows, BlockCols, LocalRows, LocalCols);
  }


};

#endif /* BTL_BLACS_INTERFACE_H */
