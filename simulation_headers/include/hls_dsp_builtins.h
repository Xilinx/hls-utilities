// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
/*
 * __VIVADO_HLS_COPYRIGHT-INFO__
 *
 *
 */

#ifndef X_DSP_BUILTINS_H
#define X_DSP_BUILTINS_H
#include <stdint.h>

#ifdef __cplusplus
#include <ap_int.h>

namespace hls {
  namespace dsp58 {
    typedef ap_int<27> A_t;
    typedef ap_int<24> B_t;
    typedef ap_int<58> C_t;
    typedef ap_int<27> D_t;

    #ifdef __DSP58_PRIMARY__
      #define __fpga_check(flags)
    #else
      #define __fpga_check(flags) static_assert(flags && false, "The DSP58 primitive is not supported on the chosen platform")
    #endif

    #define __fpga_dsp(res, op, flags, d, a, b, c, init, acc) \
      __fpga_dsp58(res, op, flags, d, a, b, c, init, acc)
    #include "etc/dsp_functions.h"
    #undef __fpga_dsp
    #undef __fpga_check

  }

  namespace dsp48e2 {
    typedef ap_int<27> A_t;
    typedef ap_int<18> B_t;
    typedef ap_int<48> C_t;
    typedef ap_int<27> D_t;

    #if defined __DSP48E2__
      #define __fpga_check(flags)
    #else
      #define __fpga_check(flags) static_assert(flags && false, "The DSP48e2 primitive is not supported on the chosen platform")
    #endif

    #define __fpga_dsp(res, op, flags, d, a, b, c, init, acc) \
      __fpga_dsp48e2(res, op, flags, d, a, b, c, init, acc)
    #include "etc/dsp_functions.h"

    #undef __fpga_dsp
    #undef __fpga_check

  }

  namespace dsp48e1 {
    typedef ap_int<25> A_t;
    typedef ap_int<18> B_t;
    typedef ap_int<48> C_t;
    typedef ap_int<25> D_t;

    #ifdef __DSP48E1__
      #define __fpga_check(flags)
    #else
      #define __fpga_check(flags) static_assert(flags && false, "The DSP48e1 primitive is not supported on the chosen platform")
    #endif

    #define __fpga_dsp(res, op, flags, d, a, b, c, init, acc) \
      __fpga_dsp48e1(res, op, flags, d, a, b, c, init, acc)
    #include "etc/dsp_functions.h"

    #undef __fpga_dsp
    #undef __fpga_check
  }

};

#endif

#endif
