#=====================================================
# Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
#=====================================================
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
testdescr = {
# (C)BLAS
'axpy' : 'y = a*x + y',
'axpby' : 'y = a*x + b*y',
'rot': 'Apply Givens rotation',
'matrix_vector': 'Matrix-Vector multiply',
'atv': 'Transposed Matrix-Vector multiply (A\' * x)',
'symv': 'Symmetric Matrix-Vector multiply',
'ger': 'Rank-1 update',
'syr2': 'Symmetric Rank-2 update',
'trisolve_vector': 'Triangular system solution',
'matrix_matrix': 'Matrix-Matrix multiply',
'aat': 'Symmetric Rank-n update',
'trisolve_matrix': 'Triangular system solution with n right hand side vectors',
'trmm': 'Triangular Matrix-Matrix multiply',

# LAPACK
'general_solve': 'Solution of a generic linear system of equations',
'least_squares': 'Least squares solution',
'lu_decomp': 'LU-decomposition',
'cholesky': 'Cholesky decomposition',
'svd_decomp': 'SVD-decomposition',
'qr_decomp': 'QR-decomposition',
'syev': 'Diagonalization of a symmetric matrix',
'stev': 'Diagonalization of a tridiagonal matrix',
'symm_ev': 'Symmetric Eigenvalue computation',

# FFTW
'FFTW_1D_Forward_Measure': 'FFTW 1D Forward (Measure)',
'FFTW_1D_Backward_Measure': 'FFTW 1D Backward (Measure)',
'FFTW_1D_Forward_Estimate': 'FFTW 1D Forward (Estimate)',
'FFTW_1D_Backward_Estimate': 'FFTW 1D Backward (Estimate)',
'FFTW_2D_Forward_Measure': 'FFTW 2D Forward (Measure)',
'FFTW_2D_Backward_Measure': 'FFTW 2D Backward (Measure)',
'FFTW_2D_Forward_Estimate': 'FFTW 2D Forward (Estimate)',
'FFTW_2D_Backward_Estimate': 'FFTW 2D Backward (Estimate)',
'FFTW_3D_Forward_Measure': 'FFTW 3D Forward (Measure)',
'FFTW_3D_Backward_Measure': 'FFTW 3D Backward (Measure)',
'FFTW_3D_Forward_Estimate': 'FFTW 3D Forward (Estimate)',
'FFTW_3D_Backward_Estimate': 'FFTW 3D Backward (Estimate)',

# METIS
'pmetis-8': 'Graph partitioning using pmetis - 8 partitions',
'kmetis-8': 'Graph partitioning using kmetis - 8 partitions',
'pmetis-64': 'Graph partitioning using pmetis - 64 partitions',
'kmetis-64': 'Graph partitioning using kmetis - 64 partitions',
}
