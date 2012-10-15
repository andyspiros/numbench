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
'MatrixVector': 'Matrix-Vector multiply',
'MatrixTVector': 'Transposed Matrix-Vector multiply (A\' * x)',
'SymMatrixVector': 'Symmetric Matrix-Vector multiply',
'Rank1Update': 'Rank-1 update',
'Rank2Update': 'Symmetric Rank-2 update',
'TriSolveVector': 'Triangular system solution',
'MatrixMatrix': 'Matrix-Matrix multiply',
'MatrixTMatrix': 'Symmetric Rank-n update',
'TriMatrixMatrix': 'Triangular Matrix-Matrix multiply',
'TriSolveMatrix': 'Triangular system solution with n right hand side vectors',

# LAPACK(e)
'GeneralSolve': 'Solution of a generic linear system of equations',
'LeastSquaresSolve': 'Least squares solution',
'LUdecomp': 'LU-decomposition',
'Choleskydecomp': 'Cholesky decomposition',
'QRdecomp': 'QR-decomposition',
'SVDdecomp': 'SVD-decomposition',

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
