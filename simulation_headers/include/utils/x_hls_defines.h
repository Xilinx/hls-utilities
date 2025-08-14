// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689


/**
 * @file x_hls_defines.h
 */

#ifndef X_HLS_DEFINES_H
#define X_HLS_DEFINES_H

#include "ap_int.h"
#include "hls_version.h"

/*
*******************************************************************************
*
* Contains global defines, macos, and structs.
*
*******************************************************************************
*/

#define PRAGMA(x) _Pragma(#x)
/*#define X_PIPELINE(ii) {\
	PRAGMA(AP PIPELINE II=ii)\
	}
*/
//#define NO_FORCING

//#define X_AP_Q_MODE AP_TRN
//#define X_AP_O_MODE AP_WRAP

/*
* May be used to specify all intantiated ap_fixed to have different options.
* Currently unused.



#define COMPARE_APPROX_SGEMM_BASE_FLOAT   0.01
#define COMPARE_APPROX_CGEMM_BASE_FLOAT   0.01
#define COMPARE_APPROX_SGEQRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_CGEQRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_SBSUT_BASE_FLOAT   0.01
#define COMPARE_APPROX_CBSUT_BASE_FLOAT   0.01
#define COMPARE_APPROX_SGETRI_BASE_FLOAT  10.0
#define COMPARE_APPROX_CGETRI_BASE_FLOAT  10.0
#define COMPARE_APPROX_SPBTRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_CPBTRF_BASE_FLOAT  0.01
#define COMPARE_APPROX_CPBTRI_BASE_FLOAT  100.0
#define COMPARE_APPROX_SGESVD_BASE_FLOAT  0.01
#define COMPARE_APPROX_CGESVD_BASE_FLOAT  1.00
#define COMPARE_APPROX_CPOSV_BASE_FLOAT  200.00

#define COMPARE_APPROX_SGEMM_BASE_FIXED   0.01
#define COMPARE_APPROX_CGEMM_BASE_FIXED   0.01
#define COMPARE_APPROX_SGEQRF_BASE_FIXED  0.01
#define COMPARE_APPROX_CGEQRF_BASE_FIXED  0.01
#define COMPARE_APPROX_SBSUT_BASE_FIXED   0.01
#define COMPARE_APPROX_CBSUT_BASE_FIXED   0.01
#define COMPARE_APPROX_SGETRI_BASE_FIXED  10.0
#define COMPARE_APPROX_CGETRI_BASE_FIXED  10.0
#define COMPARE_APPROX_SPBTRF_BASE_FIXED  0.01
#define COMPARE_APPROX_CPBTRF_BASE_FIXED  100.00
#define COMPARE_APPROX_SGESVD_BASE_FIXED  0.01
#define COMPARE_APPROX_CGESVD_BASE_FIXED  1.00

#define COMPARE_APPROX_ERROR_FLOAT 0.10 // in percentage

#define COMPARE_APPROX_ERROR_FIXED 40.00
*/

/*
* Macros
*/
//#define __HLS_MAX(x,y) ((x) > (y) ? (x) : (y))
//#define __HLS_MIN(x,y) ((x) < (y) ? (x) : (y))

/*
* Floating point data structure (with unsigned mantissa)
*/
template<int M, int E>
struct float_struct {
    ap_uint<M>  mant; // 23
    ap_uint<E> exp; // 8
    ap_uint<1> sign;
};

/*
* Floating point data structure (with signed mantissa)
*/
template<int M, int E>
struct float_struct2 {
    ap_int<M>  mant; // 24
    ap_uint<E> exp; // 8
};


#endif



