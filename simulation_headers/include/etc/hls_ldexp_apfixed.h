// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//  
// http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.


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
