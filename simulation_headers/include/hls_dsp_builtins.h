// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

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
#include "hls_stream.h"

namespace hls {
  namespace dspInternal {
    // register flags
    constexpr int64_t NO_REG            = 0x00000000;
    constexpr int64_t REG_A1            = 0x00000001;
    constexpr int64_t REG_A2            = 0x00000002;
    constexpr int64_t REG_B1            = 0x00000004;
    constexpr int64_t REG_B2            = 0x00000008;
    constexpr int64_t REG_D             = 0x00000010;
    constexpr int64_t REG_AD            = 0x00000020;
    constexpr int64_t REG_M             = 0x00000040;
    constexpr int64_t REG_C             = 0x00000080;
    constexpr int64_t REG_P             = 0x00000100;
    constexpr int64_t NO_PIPELINE       = NO_REG;
    constexpr int64_t BALANCED_PIPELINE = REG_A1 | REG_B1;
    constexpr int64_t FULL_PIPELINE     = REG_A1 | REG_A2 | REG_B1 | REG_B2 | REG_AD;

    // DSP func_index enum
    enum DSPOpCodes {
        DSP_mul_add,
        DSP_mul_sub,
        DSP_mul_rev_sub,
        DSP_add_mul_add,
        DSP_add_mul_sub,
        DSP_sub_mul_add,
        DSP_sub_mul_sub,
        DSP_mul_acc,
        DSP_add_mul_acc,
        DSP_sub_mul_rev_sub,
        DSP_cascade_mul_add,
        DSP_cascade_add_mul_add,
        DSP_add_mul_rev_sub,
        DSP_mul_add_complex,
        DSP_mul_acc_complex,
        DSP_mul_complex,
        DSP_cascade_mul_add_complex
    };
  }
  
  namespace dspcplx {
    typedef std::complex<ap_int<18>> A_t;
    typedef std::complex<ap_int<18>> B_t;
    typedef std::complex<ap_int<58>> C_t;
    typedef std::complex<ap_int<58>> D_t;
    typedef std::complex<ap_int<58>> P_t;

    typedef ap_int<36> A_tt;
    typedef ap_int<36> B_tt;
    typedef ap_int<116> C_tt;
    typedef ap_int<116> D_tt;

    #ifdef __DSP58_PRIMARY__
      #define __fpga_check(flags)
    #else
      #define __fpga_check(flags) static_assert(flags && false, "The DSPCPLX primitive is not supported on the chosen platform")
    #endif

    #define __fpga_dsp_cplx(res, op, flags, d, a, b, c, init, acc) \
      __fpga_dsp58_cplx(res, op, flags, d, a, b, c, init, acc)
    #include "etc/dsp_cplx_functions.h"
    #undef __fpga_dsp_cplx
      
    #undef __fpga_check
  }

  namespace dsp58 {
    typedef ap_int<27> A_t;
    typedef ap_int<24> B_t;
    typedef ap_int<58> C_t;
    typedef ap_int<27> D_t;
    typedef ap_int<58> P_t;
    typedef ap_int<51> M_t;

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
    typedef ap_int<48> P_t;
    typedef ap_int<45> M_t;

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
    typedef ap_int<48> P_t;
    typedef ap_int<43> M_t;

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
