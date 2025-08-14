// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#if defined __fpga_dsp_cplx && defined __fpga_check

using namespace dspInternal;

#define STAGE(reg) ((flags & reg) ? 1: 0)

namespace {
      C_t C0_cplx = C_t(0, 0);
      D_t D0_cplx = D_t(0, 0);
      C_t* accPtr_cplx = nullptr;
}

// This is the aggregated version of R_t, used internally to connect to the intrinsic
struct R_tt {
  A_tt acout;
  B_tt bcout;
  C_tt pcout;
};

struct R_t {
  A_t acout;
  B_t bcout;
  C_t pcout;
};

template <int64_t flags> 
static inline __attribute__((always_inline)) C_t
mul_add(
    A_t a,
    B_t b,
    C_t c) 
    {
      __fpga_check(flags);
      static_assert( (STAGE(REG_A1) && !STAGE(REG_A2) && STAGE(REG_B1) && !STAGE(REG_B2) && !STAGE(REG_D) && !STAGE(REG_AD) && !STAGE(REG_C))      // BALANCED_PIPELINE and do not care about M, P
                  || (STAGE(REG_A1) && STAGE(REG_A2) && STAGE(REG_B1) && STAGE(REG_B2) && !STAGE(REG_D) && STAGE(REG_AD) && !STAGE(REG_C))         // FULL_PIPELINE and do not care about M, P
                  || (!STAGE(REG_A1) && !STAGE(REG_A2) && !STAGE(REG_B1) && !STAGE(REG_B2) && !STAGE(REG_D) && !STAGE(REG_AD) && !STAGE(REG_C))    // No-pipeline and do not care about M, P
                  , "Register mode not supported, please refer to user guide"
            );
      
#ifndef __HLS_SYN__
      return static_cast<C_t>(a) * static_cast<C_t>(b) + c; 
#else
      R_tt res;
      R_t res_cplx;
      ap_int<36> A2;
      ap_int<36> B2;
      ap_int<116> C2;
      A2.range(35, 18) = a.real();
      A2.range(17, 0) = a.imag();
      B2.range(35, 18) = b.real();
      B2.range(17, 0) = b.imag();
      C2.range(115, 58) = c.real();
      C2.range(57, 0) = c.imag();
      __fpga_dsp_cplx(&res, DSP_mul_add_complex, flags, &D0_cplx, &A2, &B2, &C2, false, accPtr_cplx);
      ap_int<58> real_part = res.pcout.range(115, 58);
      ap_int<58> imag_part = res.pcout.range(57, 0);
      res_cplx.pcout.real(real_part);
      res_cplx.pcout.imag(imag_part);
      return res_cplx.pcout;
#endif
    }

template <int64_t flags> 
static inline __attribute__((always_inline)) C_t
mul(
    A_t a,
    B_t b) 
    {
      __fpga_check(flags);
      static_assert( (STAGE(REG_A1) && !STAGE(REG_A2) && STAGE(REG_B1) && !STAGE(REG_B2) && !STAGE(REG_D) && !STAGE(REG_AD) && !STAGE(REG_C))   // BALANCED_PIPELINE and do not care about M, P
                  || (STAGE(REG_A1) && STAGE(REG_A2) && STAGE(REG_B1) && STAGE(REG_B2) && !STAGE(REG_D) && STAGE(REG_AD) && !STAGE(REG_C))      // FULL_PIPELINE and do not care about M, P
                  || (!STAGE(REG_A1) && !STAGE(REG_A2) && !STAGE(REG_B1) && !STAGE(REG_B2) && !STAGE(REG_D) && !STAGE(REG_AD) && !STAGE(REG_C)) // No-pipeline and do not care about M, P
                  , "Register mode not supported, please refer to user guide"
            );
      
#ifndef __HLS_SYN__
      return static_cast<C_t>(a) * static_cast<C_t>(b); 
#else
      R_tt res;
      R_t res_cplx;
      ap_int<36> A2;
      ap_int<36> B2;
      A2.range(35, 18) = a.real();
      A2.range(17, 0) = a.imag();
      B2.range(35, 18) = b.real();
      B2.range(17, 0) = b.imag();
      __fpga_dsp_cplx(&res, DSP_mul_complex, flags, &D0_cplx, &A2, &B2, &C0_cplx, false, accPtr_cplx);
      ap_int<58> real_part = res.pcout.range(115, 58);
      ap_int<58> imag_part = res.pcout.range(57, 0);
      res_cplx.pcout.real(real_part);
      res_cplx.pcout.imag(imag_part);
      return res_cplx.pcout;
#endif
    }

template <int64_t flags> 
class acc {
  static_assert((STAGE(REG_A1) && !STAGE(REG_A2) && STAGE(REG_B1) && !STAGE(REG_B2) && !STAGE(REG_D) && !STAGE(REG_AD) && !STAGE(REG_C) && STAGE(REG_P))  // BALANCED_PIPELINE with P, do not care about M
                || (STAGE(REG_A1) && STAGE(REG_A2) && STAGE(REG_B1) && STAGE(REG_B2) && !STAGE(REG_D) && STAGE(REG_AD) && !STAGE(REG_C) && STAGE(REG_P))  // FULL_PIPELINE with P, do not care about M
                || (!STAGE(REG_A1) && !STAGE(REG_A2) && !STAGE(REG_B1) && !STAGE(REG_B2) && !STAGE(REG_D) && !STAGE(REG_AD) && !STAGE(REG_C) && STAGE(REG_P))  // No-pipeline with P, do not care about M
                , "Register mode not supported, please refer to user guide");

 private:
  C_t mAcc;
#ifndef __HLS_SYN__
  bool mHasInit = false;
  bool mIsUsed = false;
#endif

 public:
  acc() { __fpga_check(flags); }
  ~acc() {
#ifndef __HLS_SYN__
    if (mIsUsed && !mHasInit) assert(false && "Init should be used in accumulator");
#endif
  }
  C_t mul_acc(A_t a, B_t b, bool init) {
#ifndef __HLS_SYN__
    mIsUsed = true;
    if (init) {
      mHasInit = true;
      mAcc = static_cast<C_t>(a) * static_cast<C_t>(b);
    } else {
      mAcc = static_cast<C_t>(a) * static_cast<C_t>(b) + mAcc;
    }
    return mAcc;
#else
    R_tt res;
    R_t res_cplx;
    ap_int<36> A2;
    ap_int<36> B2;
    A2.range(35, 18) = a.real();
    A2.range(17, 0) = a.imag();
    B2.range(35, 18) = b.real();
    B2.range(17, 0) = b.imag();
    __fpga_dsp_cplx(&res, DSP_mul_acc_complex, flags, &D0_cplx, &A2, &B2, &C0_cplx, init, &mAcc);
    ap_int<58> real_part = res.pcout.range(115, 58);
    ap_int<58> imag_part = res.pcout.range(57, 0);
    res_cplx.pcout.real(real_part);
    res_cplx.pcout.imag(imag_part);
    return res_cplx.pcout;
#endif
  }
};

template <int64_t flags>
class cascade {
 private:
  C_t mState;
#ifndef __HLS_SYN__
 private:
  A_t a1_reg_;
  B_t b1_reg_;
  A_t a2_reg_;
  B_t b2_reg_;

  void dsp_reg_shift(A_t a, B_t b) {
    a2_reg_ = a1_reg_;
    a1_reg_ = a;
    b2_reg_ = b1_reg_;
    b1_reg_ = b;
  };
#endif

 public:
  cascade() {
    __fpga_check(flags);
#ifndef __HLS_SYN__
    a1_reg_ = 0;
    b1_reg_ = 0;
    a2_reg_ = 0;
    b2_reg_ = 0;
#endif
  };

  R_t mul_add(A_t a, B_t b, C_t c) {
    static_assert(STAGE(REG_A1) || STAGE(REG_A2), "Register A1 or A2 is required for DSP cascade mul_add");
    static_assert(STAGE(REG_B1) || STAGE(REG_B2), "Register B1 or B2 is required for DSP cascade  mul_add");
    static_assert(STAGE(REG_M), "Register M is required for DSP cascade mul_add");
    static_assert(STAGE(REG_P), "Register P is required for DSP cascade mul_add");
    static_assert(!STAGE(REG_D), "Register D is not supported for cascade mul_add");
    static_assert(!STAGE(REG_C), "Register C is not supported for DSP cascade mul_add");
    R_t res_cplx;
#ifndef __HLS_SYN__
    dsp_reg_shift(a, b);
    if (STAGE(REG_A1) && STAGE(REG_A2)) {
      res_cplx.acout = a2_reg_;
    } else {
      res_cplx.acout = a1_reg_;
    }
    if (STAGE(REG_B1) && STAGE(REG_B2)) {
      res_cplx.bcout = b2_reg_;
    } else {
      res_cplx.bcout = b1_reg_;
    }
    res_cplx.pcout = static_cast<C_t>(res_cplx.acout) * static_cast<C_t>(res_cplx.bcout) + c;
#else
    R_tt res;
    ap_int<36> A2;
    ap_int<36> B2;
    ap_int<116> C2;
    A2.range(35, 18) = a.real();
    A2.range(17, 0) = a.imag();
    B2.range(35, 18) = b.real();
    B2.range(17, 0) = b.imag();
    C2.range(115, 58) = c.real();
    C2.range(57, 0) = c.imag();
    __fpga_dsp_cplx(&res, DSP_cascade_mul_add_complex, flags, &D0_cplx, &A2, &B2, &C2, false, accPtr_cplx);
    ap_int<58> pcout_real = res.pcout.range(115, 58);
    ap_int<58> pcout_imag = res.pcout.range(57, 0);
    res_cplx.pcout.real(pcout_real);
    res_cplx.pcout.imag(pcout_imag);
    ap_int<36> acout_real = res.acout.range(35, 18);
    ap_int<36> acout_imag = res.acout.range(17, 0);
    res_cplx.acout.real(acout_real);
    res_cplx.acout.imag(acout_imag);
    ap_int<36> bcout_real = res.bcout.range(35, 18);
    ap_int<36> bcout_imag = res.bcout.range(17, 0);
    res_cplx.bcout.real(bcout_real);
    res_cplx.bcout.imag(bcout_imag);
#endif
    return res_cplx;
  }
};
#undef STAGE
#endif
