// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

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
