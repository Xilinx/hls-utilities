// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#if defined __fpga_dsp && defined __fpga_check

using namespace dspInternal;

#define STAGE(reg) ((flags&reg)? 1: 0)

namespace {
      C_t C0 = 0;
      D_t D0 = 0;
      C_t* accPtr = nullptr;
}

struct R_t {
  A_t acout;
  B_t bcout;
  C_t pcout;
};

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
mul_add(
    A_t a,
    B_t b,
    C_t c) 
    {
      __fpga_check(flags);
      static_assert(!STAGE(REG_D), "Register D is not supported for mul_add operation");
      static_assert(!STAGE(REG_AD), "Register AD is not supported for mul_add operation");
#ifndef __HLS_SYN__
      return a * b + c; 
#else
      R_t res;
      __fpga_dsp(&res, DSP_mul_add, flags, &D0, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
mul_sub(
    A_t a,
    B_t b,
    C_t c)
    {
      __fpga_check(flags);
      static_assert(!STAGE(REG_D), "Register D is not supported for mul_sub operation");
      static_assert(!STAGE(REG_AD), "Register AD is not supported for mul_sub operation");
#ifndef __HLS_SYN__
      return a * b - c;
#else
      R_t res;
      __fpga_dsp(&res, DSP_mul_sub, flags, &D0, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
mul_rev_sub(
    A_t a,
    B_t b,
    C_t c)
    {
      __fpga_check(flags);
      static_assert(!STAGE(REG_D), "Register D is not supported for mul_rev_sub operation");
      static_assert(!STAGE(REG_AD) , "Register AD is not supported for mul_rev_sub operation");
#ifndef __HLS_SYN__
      return c - a * b;
#else
      R_t res;
      __fpga_dsp(&res, DSP_mul_rev_sub, flags, &D0, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
add_mul_add(
    D_t d,
    A_t a,
    B_t b,
    C_t c) 
    {
      __fpga_check(flags);
#ifndef __HLS_SYN__
      return (a + d) * b + c;
#else
      R_t res;
      __fpga_dsp(&res, DSP_add_mul_add, flags, &d, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
add_mul_sub(
    D_t d,
    A_t a,
    B_t b,
    C_t c) 
    {
      __fpga_check(flags);
#ifndef __HLS_SYN__
      return (a + d) * b - c;
#else
      R_t res;
      __fpga_dsp(&res, DSP_add_mul_sub, flags, &d, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
sub_mul_add(
    D_t d,
    A_t a,
    B_t b,
    C_t c) 
    {
      __fpga_check(flags);
#ifndef __HLS_SYN__
      return (d - a) * b + c;
#else
      R_t res;
      __fpga_dsp(&res, DSP_sub_mul_add, flags, &d, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
sub_mul_sub(
    D_t d,
    A_t a,
    B_t b,
    C_t c) 
    {
      __fpga_check(flags);
#ifndef __HLS_SYN__
      return (d - a) * b - c;
#else
      R_t res;
      __fpga_dsp(&res, DSP_sub_mul_sub, flags, &d, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
add_mul_rev_sub(
    D_t d,    
    A_t a,
    B_t b,
    C_t c)
    {
      __fpga_check(flags);
#ifndef __HLS_SYN__
      return c - (d+a) * b;
#else
      R_t res;
      __fpga_dsp(&res, DSP_add_mul_rev_sub, flags, &d, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template<int64_t flags> 
static inline __attribute__((always_inline)) C_t
sub_mul_rev_sub(
    D_t d,    
    A_t a,
    B_t b,
    C_t c)
    {
      __fpga_check(flags);
#ifndef __HLS_SYN__
      return c - (d-a) * b;
#else
      R_t res;
      __fpga_dsp(&res, DSP_sub_mul_rev_sub, flags, &d, &a, &b, &c, false, accPtr);
      return res.pcout;
#endif
    }

template <int64_t flags> 
class acc {
  static_assert(STAGE(REG_P), "Register P must be used for accumulation");

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
      mAcc = (C_t)(a * b);
    } else {
      mAcc = (C_t)(a * b + mAcc);
    }
    return mAcc;
#else
    R_t res;
    __fpga_dsp(&res, DSP_mul_acc, flags, &D0, &a, &b, &C0, init, &mAcc);
    return res.pcout;
#endif
  }

  C_t add_mul_acc(D_t d, A_t a, B_t b, bool init) {
#ifndef __HLS_SYN__
    mIsUsed = true;
    if (init) {
      mHasInit = true;
      mAcc = (C_t)((a + d) * b);
    } else {
      mAcc = ((a + d) * b + mAcc);
    }
    return mAcc;
#else
    R_t res;
    __fpga_dsp(&res, DSP_add_mul_acc, flags, &d, &a, &b, &C0, init, &mAcc);
    return res.pcout;
#endif
  }
};

template <int64_t flags> 
class cascade {
private:
      C_t mState;
#ifndef __HLS_SYN__
private : 
  A_t a1_reg_;
  B_t b1_reg_;
  A_t a2_reg_;
  B_t b2_reg_;

  void dsp_reg_shift(A_t a, B_t b){
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

   R_t mul_add(
    A_t a,
    B_t b,
    C_t c){
      static_assert(STAGE(REG_A1) || STAGE(REG_A2) , "Register A1 or A2 is required for using DSP cascade mul_add");
      static_assert(STAGE(REG_B1) || STAGE(REG_B2), "Register B1 or B2 is required for DSP cascade  mul_add");
      static_assert(STAGE(REG_M), "Register M is required for DSP cascade mul_add");
      static_assert(STAGE(REG_P), "Register P is required for DSP cascade mul_add");
      static_assert(!STAGE(REG_D), "Register D is not supported for cascade mul_add");
      static_assert(!STAGE(REG_AD), "Register AD is not supported for cascade mul_add");
      static_assert(!STAGE(REG_C), "Register C is not supported for DSP cascade mul_add");
    R_t res;
#ifndef __HLS_SYN__
      dsp_reg_shift(a, b);
      if(STAGE(REG_A1) && STAGE(REG_A2)){
            res.acout = a2_reg_;
      } else {
            res.acout = a1_reg_;
      }
      if(STAGE(REG_B1) && STAGE(REG_B2)){
            res.bcout = b2_reg_;
      } else {
            res.bcout = b1_reg_;
      }
      res.pcout = res.acout * res.bcout + c;
#else
    __fpga_dsp(&res, DSP_cascade_mul_add, flags, &D0, &a, &b, &c, false, &mState);
#endif
    return res;
  }
};
#undef STAGE
#endif
