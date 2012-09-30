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
#ifndef CDECLARATIONS_HPP_
#define CDECLARATIONS_HPP_


// Cblas constants
const int CblasRowMajor = 101;
const int CblasColMajor = 102;

const int CblasNoTrans = 111;
const int CblasTrans = 112;
const int CblasConjTrans = 113;
const int AtlasConj = 114;

const int CblasUpper = 121;
const int CblasLower = 122;

const int CblasNonUnit = 131;
const int CblasUnit = 132;

const int CblasLeft = 141;
const int CblasRight = 142;


// Cblas functions
extern "C" {



/****************
 * LEVEL 1 BLAS *
 ****************/

    void cblas_srot(int, float*, int, float*, int, float, float);
    void cblas_drot(int, double*, int, double*, int, double, double);

    void cblas_saxpy(int, float, const float*, int, float*, int);
    void cblas_daxpy(int, double, const double*, int, double*, int);

    float cblas_sdot(int, const float*, int, const float*, int);
    double cblas_ddot(int, const double*, int, const double*, int);

    float cblas_snrm2(int, const float*, int);
    double cblas_dnrm2(int, const double*, int);




    /****************
     * LEVEL 2 BLAS *
     ****************/

    void cblas_sgemv(int, int, int, int, float, const float*, int, const float*, int, float, float*, int);
    void cblas_dgemv(int, int, int, int, double, const double*, int, const double*, int, double, double*, int);

    void cblas_ssymv(int, int, int, float, const float*, int, const float*, int, float, float*, int);
    void cblas_dsymv(int, int, int, double, const double*, int, const double*, int, double, double*, int);

    void cblas_strmv(int, int, int, int, int, const float*, int, float*, int);
    void cblas_dtrmv(int, int, int, int, int, const double*, int, double*, int);

    void cblas_strsv(int, int, int, int, int, const float*, int, float*, int);
    void cblas_dtrsv(int, int, int, int, int, const double*, int, double*, int);

    void cblas_sger(int, int, int, float, const float*, int, const float*, int, float*, int);
    void cblas_dger(int, int, int, double, const double*, int, const double*, int, double*, int);

    void cblas_ssyr2(int, int, int, float, const float*, int, const float*, int, float*, int);
    void cblas_dsyr2(int, int, int, double, const double*, int, const double*, int, double*, int);




    /****************
     * LEVEL 3 BLAS *
     ****************/

    void cblas_sgemm(int, int, int, int, int, int, float, const float*, int, const float*, int, float, float*, int);
    void cblas_dgemm(int, int, int, int, int, int, double, const double*, int, const double*, int, double, double*, int);

    void cblas_strmm(int, int, int, int, int, int, int, float, const float*, int, float*, int);
    void cblas_dtrmm(int, int, int, int, int, int, int, double, const double*, int, double*, int);

    void cblas_strsm(int, int, int, int, int, int, int, float, const float*, int, float*, int);
    void cblas_dtrsm(int, int, int, int, int, int, int, double, const double*, int, double*, int);

}


#endif /* CDECLARATIONS_HPP_ */
