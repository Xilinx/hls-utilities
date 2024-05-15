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

#ifndef X_DSP_DP_BUILTINS_H
#define X_DSP_DP_BUILTINS_H

// User interfaces.
// Result += A1in * B1in + A2in * B2in + (clear ? 0 : Accum);
static inline int32_t __builtin_mac16x2(
    int16_t A1in,
    int16_t A2in,
    int16_t B1in,
    int16_t B2in,
    int32_t Accum,
    __dsp1_t clear,
    __dsp1_t ap_clk_div2) __attribute__((always_inline));

// This builtin is only used for speicial cases.
// Result = ((A1in << 21) + signed_ext_to_27bit(B1in)) * C1in +
//          ((A2in << 21) + signed_ext_to 27bit(B2in)) * C2in + (clear ? 0 : Accum);
static inline __dsp48_t __builtin_mac6x2_mac8x1(
    __dsp6_t A1in,
    __dsp6_t A2in,
    int8_t B1in,
    int8_t B2in,
    int8_t C1in,
    int8_t C2in,
    __dsp48_t Accum,
    __dsp1_t clear,
    __dsp1_t ap_clk_div2) __attribute__((always_inline));

// HLS Internal functions, don't use them directly.
#ifdef __HLS_SYN__
#ifdef __cplusplus
extern "C" {
#endif

int32_t _ssdm_op_Mac16_Clockx2(
    int __attribute__((bitwidth(1))) clockx2,
    int16_t A1in,
    int16_t A2in,
    int16_t B1in,
    int16_t B2in,
    int32_t Accum,
    int __attribute__((bitwidth(1))) clear);

int64_t __attribute__((bitwidth(48))) 
_ssdm_op_Mac6_Mac8_Clockx2(
    int __attribute__((bitwidth(1))) clockx2,
    int32_t __attribute__((bitwidth(6))) A1in,
    int32_t __attribute__((bitwidth(6))) A2in,
    int8_t B1in,
    int8_t B2in,
    int8_t C1in,
    int8_t C2in,
    int64_t __attribute__((bitwidth(48))) Accum,
    int __attribute__((bitwidth(1))) clear);

#ifdef __cplusplus
}
#endif
#endif

int32_t __builtin_mac16x2(
    int16_t A1in,
    int16_t A2in,
    int16_t B1in,
    int16_t B2in,
    int32_t Accum,
    __dsp1_t clear,
    __dsp1_t ap_clk_div2) {
#ifdef __HLS_SYN__
  return _ssdm_op_Mac16_Clockx2(ap_clk_div2, A1in, A2in, B1in, B2in, 0, clear);
#else
  return A1in * B1in + A2in * B2in + (clear ? 0 : Accum); 
#endif
}

__dsp48_t __builtin_mac6x2_mac8x1(
    __dsp6_t A1in,
    __dsp6_t A2in,
    int8_t B1in,
    int8_t B2in,
    int8_t C1in,
    int8_t C2in,
    __dsp48_t Accum,
    __dsp1_t clear,
    __dsp1_t ap_clk_div2) {
#ifdef __HLS_SYN__
  return (long long)_ssdm_op_Mac6_Mac8_Clockx2(ap_clk_div2, A1in, A2in, B1in, B2in, C1in, C2in, 0, clear);
#else
  int64_t A1 = A1in;
  int64_t A2 = A2in;
  int64_t B1 = B1in;
  int64_t B2 = B2in;
  int64_t C1 = C1in;
  int64_t C2 = C2in;
  A1 = A1 << 20;
  A2 = A2 << 20;
  int64_t AccumN = Accum;
  int64_t Result = (A1 + B1) * C1 + (A2 + B2) * C2 + (clear ? 0 : AccumN);
  return Result;
#endif
}

#endif

