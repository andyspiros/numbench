//=====================================================
// Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
//=====================================================
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#define BLAS_FUNC(NAME) CAT(cblas_,CAT(SCALAR_PREFIX,NAME))

template<> class blas_interface<SCALAR> : public c_interface_base<SCALAR>
{

public :
  
  static SCALAR fone;
  static SCALAR fzero;

  static inline std::string name()
  {
    return MAKE_STRING(CBLASNAME);
  }

  static inline void matrix_vector_product(gene_matrix & A, gene_vector & B, gene_vector & X, int N){
    BLAS_FUNC(gemv)(CblasColMajor,CblasNoTrans,N,N,fone,A,N,B,intone,fzero,X,intone);
  }

  static inline void symv(gene_matrix & A, gene_vector & B, gene_vector & X, int N){
    BLAS_FUNC(symv)(CblasColMajor,CblasLower,N,fone,A,N,B,intone,fzero,X,intone);
  }

  static inline void syr2(gene_matrix & A, gene_vector & B, gene_vector & X, int N){
    BLAS_FUNC(syr2)(CblasColMajor,CblasLower,N,fone,B,intone,X,intone,A,N);
  }

  static inline void ger(gene_matrix & A, gene_vector & X, gene_vector & Y, int N){
    BLAS_FUNC(ger)(CblasColMajor,N,N,fone,X,intone,Y,intone,A,N);
  }

  static inline void rot(gene_vector & A,  gene_vector & B, SCALAR c, SCALAR s, int N){
    BLAS_FUNC(rot)(N,A,intone,B,intone,c,s);
  }

  static inline void atv_product(gene_matrix & A, gene_vector & B, gene_vector & X, int N){
    BLAS_FUNC(gemv)(CblasColMajor,CblasTrans,N,N,fone,A,N,B,intone,fzero,X,intone);
  }

  static inline void matrix_matrix_product(gene_matrix & A, gene_matrix & B, gene_matrix & X, int N){
    BLAS_FUNC(gemm)(CblasColMajor,CblasNoTrans,CblasNoTrans,N,N,N,fone,A,N,B,N,fzero,X,N);
  }

  static inline void transposed_matrix_matrix_product(gene_matrix & A, gene_matrix & B, gene_matrix & X, int N){
    BLAS_FUNC(gemm)(CblasColMajor,CblasTrans,CblasNoTrans,N,N,N,fone,A,N,B,N,fzero,X,N);
  }

  static inline void aat_product(gene_matrix & A, gene_matrix & X, int N){
    BLAS_FUNC(syrk)(CblasColMajor,CblasLower,CblasNoTrans,N,N,fone,A,N,fzero,X,N);
  }

  static inline void axpy(SCALAR coef, const gene_vector & X, gene_vector & Y, int N){
    BLAS_FUNC(axpy)(N,coef,X,intone,Y,intone);
  }

  static inline void axpby(SCALAR a, const gene_vector & X, SCALAR b, gene_vector & Y, int N){
    BLAS_FUNC(scal)(N,b,Y,intone);
    BLAS_FUNC(axpy)(N,a,X,intone,Y,intone);
  }

  static inline void trisolve_lower(const gene_matrix & L, const gene_vector& B, gene_vector & X, int N){
    BLAS_FUNC(copy)(N, B, intone, X, intone);
    BLAS_FUNC(trsv)(CblasColMajor,CblasLower, CblasNoTrans, CblasNonUnit, N, L, N, X, intone);
  }

  static inline void trisolve_lower_matrix(const gene_matrix & L, const gene_matrix& B, gene_matrix & X, int N){
    BLAS_FUNC(copy)(N, B, intone, X, intone);
    BLAS_FUNC(trsm)(CblasColMajor,CblasRight, CblasLower, CblasNoTrans, CblasNonUnit, N, N, fone, L, N, X, N);
  }

  static inline void trmm(gene_matrix & A, gene_matrix & B, gene_matrix & X, int N){
    BLAS_FUNC(trmm)(CblasColMajor,CblasLeft, CblasLower, CblasNoTrans,CblasNonUnit, N,N,fone,A,N,B,N);
  }

};

SCALAR blas_interface<SCALAR>::fone = SCALAR(1);
SCALAR blas_interface<SCALAR>::fzero = SCALAR(0);
