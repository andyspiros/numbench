//=====================================================
// Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
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
#ifndef FORTRANDECLARATIONS_HPP_
#define FORTRANDECLARATIONS_HPP_

extern "C" {



    /****************
     * LEVEL 1 BLAS *
     ****************/

    void srot_(const int*, float*, const int*, float*, const int*, const float*, const float*);
    void drot_(const int*, double*, const int*, double*, const int*, const double*, const double*);

    void saxpy_(const int*, const float*, const float*, const int*, float*, const int*);
    void daxpy_(const int*, const double*, const double*, const int*, double*, const int*);

    float sdot_(const int*, const float*, const int*, const float*, const int*);
    double ddot_(const int*, const double*, const int*, const double*, const int*);

    float snrm2_(const int*, const float*, const int*);
    double dnrm2_(const int*, const double*, const int*);




    /****************
     * LEVEL 2 BLAS *
     ****************/

    void sgemv_(const char*, const int*, const int*, const float*, const float*, const int*, const float*, const int*, const float*, float*, const int*);
    void dgemv_(const char*, const int*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);

    void ssymv_(const char*, const int*, const float*, const float*, const int*, const float*, const int*, const float*, float*, const int*);
    void dsymv_(const char*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);

    void strmv_(const char*, const char*, const char*, const int*, const float*, const int*, float*, const int*);
    void dtrmv_(const char*, const char*, const char*, const int*, const double*, const int*, double*, const int*);

    void strsv_(const char*, const char*, const char*, const int*, const float*, const int*, float*, const int*);
    void dtrsv_(const char*, const char*, const char*, const int*, const double*, const int*, double*, const int*);

    void sger_(const int*, const int*, const float*, const float*, const int*, const float*, const int*, float*, const int*);
    void dger_(const int*, const int*, const double*, const double*, const int*, const double*, const int*, double*, const int*);

    void ssyr2_(const char*, const int*, const float*, const float*, const int*, const float*, const int*, float*, const int*);
    void dsyr2_(const char*, const int*, const double*, const double*, const int*, const double*, const int*, double*, const int*);




    /****************
     * LEVEL 3 BLAS *
     ****************/

    void sgemm_(const char*, const char*, const int*, const int*, const int*, const float*, const float*, const int*, const float*, const int*, const float*, float*, const int*);
    void dgemm_(const char*, const char*, const int*, const int*, const int*, const double*, const double*, const int*, const double*, const int*, const double*, double*, const int*);

    void strmm_(const char*, const char*, const char*, const char*, const int*, const int*, const float*, const float*, const int*, float*, const int*);
    void dtrmm_(const char*, const char*, const char*, const char*, const int*, const int*, const double*, const double*, const int*, double*, const int*);

    void strsm_(const char*, const char*, const char*, const char*, const int*, const int*, const float*, const float*, const int*, float*, const int*);
    void dtrsm_(const char*, const char*, const char*, const char*, const int*, const int*, const double*, const double*, const int*, double*, const int*);




    /******************
     * LAPACK SOLVERS *
     ******************/

    void sgesv_(const int*, const int*, float*, const int*, int*, float*, const int*, int*);
    void dgesv_(const int*, const int*, double*, const int*, int*, double*, const int*, int*);

    void sgels_(const char*, const int*, const int*, const int*, float*, const int*, float*, const int*, float*, const int*, int*);
    void dgels_(const char*, const int*, const int*, const int*, double*, const int*, double*, const int*, double*, const int*, int*);




    /*************************
     * LAPACK FACTORIZATIONS *
     *************************/

    void sgetrf_(const int*, const int*, float*, const int*, int*, int*);
    void dgetrf_(const int*, const int*, double*, const int*, int*, int*);

    void spotrf_(const char*, const int*, float*, const int*, int*);
    void dpotrf_(const char*, const int*, double*, const int*, int*);


}


#endif /* FORTRANDECLARATIONS_HPP_ */

