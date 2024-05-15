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


#ifndef _X_HLS_LGAMMA_APFIXED_H_
#define _X_HLS_LGAMMA_APFIXED_H_
#include "ap_fixed.h"
#include "ap_int.h"
#include "utils/x_hls_defines.h"
#include "utils/x_hls_traits.h"
#include "utils/x_hls_utils.h"

namespace lgamma_fixed {

template <int W, int I> ap_fixed<W, I> lgamma_generic(ap_fixed<W, I> x_fixed) {
#pragma HLS pipeline

  double x = x_fixed.to_double();
  double r = hls::lgamma(x);
  fp_struct<double> out(r);
  ap_ufixed<W+1, I+1> ret = 1;
  int F = W - I;
  if (F > 52) F = 52;
  if (W > I) ret(F - 1, 0) = out.sig(52 - 1, 52 - F);
  ret = (ret << (out.exp - fp_struct<double>::EXP_BIAS));
  return ap_fixed<W, I>(ret);
}

template <int W, int I> ap_ufixed<W, I> lgamma_generic(ap_ufixed<W, I> x) {
  ap_fixed<W + 1, I + 1> xf = x;
  return lgamma_generic(xf);
}
template <int I_> ap_int<I_> lgamma_generic(ap_int<I_> x) {
  ap_fixed<I_, I_> xf = x;
  return lgamma_generic(xf);
}
template <int I_> ap_uint<I_> lgamma_generic(ap_uint<I_> x) {
  ap_fixed<I_ + 1, I_ + 1> xf = x;
  return lgamma_generic(xf);
}

//lgamma_r
template <int W, int I> ap_fixed<W, I> lgamma_r_generic(ap_fixed<W, I> x, int *signgamp) {
  *signgamp = 1;
  return lgamma_generic(x);
}

template <int W, int I> ap_ufixed<W, I> lgamma_r_generic(ap_ufixed<W, I> x, int *signgamp) {
  ap_fixed<W + 1, I + 1> xf = x;
  return lgamma_r_generic(xf, signgamp);
}

template <int I_> ap_int<I_> lgamma_r_generic(ap_int<I_> x, int *signgamp) {
  ap_fixed<I_, I_> xf = x;
  return lgamma_r_generic(xf, signgamp);
}

template <int I_> ap_uint<I_> lgamma_r_generic(ap_uint<I_> x, int *signgamp) {
  ap_fixed<I_ + 1, I_ + 1> xf = x;
  return lgamma_r_generic(xf, signgamp);
}

} // lgamma_fixed

#endif
