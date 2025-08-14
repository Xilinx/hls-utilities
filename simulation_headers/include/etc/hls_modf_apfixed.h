// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689


/**
 * @file hls_modf_ap_fixed.h
 * @brief implement modf function for ap_fixed data type.
 */
#ifndef _X_HLS_MODF_APFIXED_H_
#define _X_HLS_MODF_APFIXED_H_
#include "etc/hls_round_copysign_apfixed.h"
namespace modf_internal {
template<int W, int I>
ap_fixed<W,I> modf(ap_fixed<W, I>  x,
                   ap_fixed<W, I>* int_part){
    ap_fixed<W, I> frac_part = 0;
    if(W == I) {
        *int_part = x;
        frac_part = 0;
    } else if(I > 0) {
        ap_fixed<W+1, I+1> xi = x;
        ap_ufixed<W, I> x_p = fabs_fixed(xi);
        frac_part(W - I - 1,0) = x_p(W - I - 1,0);
        if(xi[W-1]) frac_part = - frac_part;
        *int_part = x_p(W-1, W-I);
        if(xi[W-1]) *int_part = -*int_part;
    } else {
       *int_part = 0;
       frac_part = x;
    }
    return frac_part;
}
template<int W, int I>
ap_ufixed<W,I> modf(ap_ufixed<W, I>  x,
                    ap_ufixed<W, I>* int_part){
    ap_fixed<W+1, I+1> xi = x;
    ap_fixed<W+1, I+1> ii = 0; 
    ap_fixed<W+1, I+1> r = modf(xi, &ii);
    *int_part = ii;
    return r;
} 
template<int I>
ap_int<I> modf(ap_int<I>  x,
               ap_int<I>* int_part){
    *int_part = x;
    return 0;
} 
template<int I>
ap_uint<I> modf(ap_uint<I>  x,
                ap_uint<I>* int_part){
    *int_part = x;
    return 0;
} 
}
#endif//ifndef _X_HLS_MODF_APFIXED_H_


