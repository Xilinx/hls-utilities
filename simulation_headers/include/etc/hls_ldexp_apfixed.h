// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef _X_HLS_LDEXP_APFIXED_H_
#define _X_HLS_LDEXP_APFIXED_H_
#include "ap_fixed.h"

namespace hls_ldexp {

template<int W, int I>
ap_fixed<W,I> ldexp(ap_fixed<W,I> x, int exp) {
    ap_fixed<W,I> result;
    if(exp >= W || exp <= -W) {
        result = 0;
    } else {
        result = x << exp;
    }
    return result;
}

template<int W, int I>
ap_ufixed<W,I> ldexp(ap_ufixed<W,I> x, int exp) {
    ap_fixed<W+1,I+1> xf = x;
    return ldexp(xf, exp);
}

template<int I>
ap_int<I> ldexp(ap_int<I> x, int exp) {
    ap_fixed<I,I> xf = x;
    return ldexp(xf, exp);
}

template<int I>
ap_uint<I> ldexp(ap_uint<I> x, int exp) {
    ap_fixed<I+1,I+1> xf = x;
    return ldexp(xf, exp);
}

template<int W, int I>
ap_fixed<W,I> scalbn(ap_fixed<W,I> x, int n) {
    ap_fixed<W,I> result;
    if(n >= W || n <= -W) {
        result = 0;
    } else {
        result = x << n;
    }
    return result;
}

template<int W, int I>
ap_ufixed<W,I> scalbn(ap_ufixed<W,I> x, int n) {
    ap_fixed<W+1,I+1> xf = x;
    return scalbn(xf, n);
}

template<int I>
ap_int<I> scalbn(ap_int<I> x, int n) {
    ap_fixed<I,I> xf = x;
    return scalbn(xf, n);
}

template<int I>
ap_uint<I> scalbn(ap_uint<I> x, int n) {
    ap_fixed<I+1,I+1> xf = x;
    return scalbn(xf, n);
}

template<int W, int I>
ap_fixed<W,I> scalbln(ap_fixed<W,I> x, long int n) {
    ap_fixed<W,I> result;
    if(n >= W || n <= -W) {
        result = 0;
    } else {
        result = x << n;
    }
    return result;
}

template<int W, int I>
ap_ufixed<W,I> scalbln(ap_ufixed<W,I> x, long int n) {
    ap_fixed<W+1,I+1> xf = x;
    return scalbln(xf, n);
}

template<int I>
ap_int<I> scalbln(ap_int<I> x, long int n) {
    ap_fixed<I,I> xf = x;
    return scalbln(xf, n);
}

template<int I>
ap_uint<I> scalbln(ap_uint<I> x, long int n) {
    ap_fixed<I+1,I+1> xf = x;
    return scalbln(xf, n);
}

}

#endif
