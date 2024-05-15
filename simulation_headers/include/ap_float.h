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

#ifndef __AP_FLOAT_H__
#define __AP_FLOAT_H__

#include <etc/ap_common.h>

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_math.h>

#ifndef __SYNTHESIS__
#include <floating_point_v7_1_bitacc_cmodel.h>
#endif

#include <limits>
#ifndef __SYNTHESIS__
#include <iostream>
#endif

static constexpr int log2_ceil(int val) {
  if (val <= 0)
    return std::numeric_limits<int>::lowest();
  if (val == 1)
    return 0;
  //return 32 - __builtin_clz(val - 1);
  return log2_ceil((val + 1) / 2) + 1;
}

/** Arbitrary Precision Floating-Point Class
 *   - W: Total bitwidth (half = 16, float = 32, double = 64)
 *   - E: Exponent's bitwidth (half = 5, float = 8, double = 11)
 */
template<int W, int E>
class ap_float {
  // Internal representation stored into a single ap_int<W>
  // Note the ap_int<W> alignment requirement. (aligned to next power-of-two)
  ap_int<W> bits;

  static constexpr int M = W - E; // note that this includes the "hidden" bit
  static_assert(E >= 4, "ap_float exponent must be at least 4 bits");
  static_assert(E <= 16, "ap_float exponent must be at most 16 bits");
  static_assert(M >= 4, "ap_float significand must be at least 4 bits");
  static_assert(M <= 64, "ap_float significand must be at most 64 bits");
  static_assert(E >= log2_ceil(M+3)+1, "ap_float exponent must be at least "
                                        "ceil(log2(significand+3))+1 bits");

  // Internal data type
  using bits_t = ap_int<W>;

  // Internal reference types
  using bits_bit_ref_t = ap_bit_ref<bits_t::width, bits_t::sign_flag>;
  using bits_range_ref_t = ap_range_ref<bits_t::width, bits_t::sign_flag>;

#ifndef __SYNTHESIS__
  // Raw conversion from double (rounding not supported)
  void from_double(const double &x) {
    static_assert(W <= 64, "Raw conversion from double supported up to 64 total bits");
    static_assert(M <= 53, "Raw conversion from double supported up to 53 mantissa bits");
    static_assert(E <= 11, "Raw conversion from double supported up to 11 exponent bits");
    ap_float<64, 11> val;
    val = *reinterpret_cast<const ap_float<64, 11>*>(&x);
    sign_ref() = val.sign_ref();

    if (val.exponent_ref() == val.exponent_denorm()) {
      // Sub-normals rounded to 0
      exponent_ref() = exponent_denorm();
      mantissa_ref() = 0;
    } else if (val.exponent_ref() == val.exponent_infinity()) {
      if (val.mantissa_ref() == 0) {
        // Infinite stay infinite
        exponent_ref() = exponent_infinity();
        mantissa_ref() = 0;
      } else {
        // NaN become Canonical QNaN
        exponent_ref() = exponent_infinity();
        mantissa_ref() = -1;
      }
    } else {
      // Normals are assumed to fit in the range (rounding must be done before)
      set_unbiased_exponent(val.get_unbiased_exponent());
      mantissa_ref() = ap_int<M-1>(val.mantissa_ref() >> (53-M));
    }
  }

  // Raw conversion to double (rounding not supported)
  double to_double() const {
    static_assert(W <= 64, "Raw conversion to double supported up to 64 total bits");
    static_assert(M <= 53, "Raw conversion to double supported up to 53 mantissa bits");
    static_assert(E <= 11, "Raw conversion to double supported up to 11 exponent bits");
    ap_float<64, 11> val;
    val.sign_ref() = sign_ref();

    if (exponent_ref() == exponent_denorm()) {
      // Sub-normals rounded to 0
      val.exponent_ref() = val.exponent_denorm();
      val.mantissa_ref() = 0;
    } else if (exponent_ref() == exponent_infinity()) {
      if (mantissa_ref() == 0) {
        // Infinite stay infinite
        val.exponent_ref() = val.exponent_infinity();
        val.mantissa_ref() = 0;
      } else {
        // NaN become Canonical QNaN
        val.exponent_ref() = val.exponent_infinity();
        val.mantissa_ref() = -1;
      }
    } else {
      // Normals are assumed to fit in the range (rounding must be done before)
      val.set_unbiased_exponent(get_unbiased_exponent());
      val.mantissa_ref() = ap_int<64>(mantissa_ref()) << (53-M);
    }
    return *reinterpret_cast<double*>(&val);
  }

  // Initialize the xip_fpo_t to match precision/bitwidth
  void init_xip_fpo_t(xip_fpo_t &res) const {
    xip_fpo_init2(res, E, M);
  }

  // Conversion to xip_fpo_t
  void to_xip_fpo_t(xip_fpo_t &res) const {
    // FIXME Raw double conversion may cause weird rounding
    xip_fpo_set_d(res, to_double());
  }

  // Conversion from xip_fpo_t
  void from_xip_fpo_t(const xip_fpo_t &f) {
    // FIXME Raw double conversion may cause weird rounding
    from_double(xip_fpo_get_d(f));
  }

  // Clear the xip_fpo_t to free memory
  void fini_xip_fpo_t(xip_fpo_t &res) const {
    xip_fpo_clear(res);
  }
#endif

public:
  // Sign, Exponent and Mantissa (value types)
  using sign_t = ap_int<1>;
  using exponent_t = ap_uint<E>;
  using mantissa_t = ap_uint<M-1>;

  // Sign, Exponent and Mantissa (reference types) (to avoid unnecessary copies)
  using sign_ref_t = bits_bit_ref_t;
  using exponent_ref_t = bits_range_ref_t;
  using mantissa_ref_t = bits_range_ref_t;

  // Clean names for bitwidth values
  static constexpr int width = W;
  static constexpr int exponent_width = E;
  static constexpr int mantissa_width = M;

  // All constructor, copy-constructor, copy-assignment are default
  ap_float() = default;
  ~ap_float() = default;
  ap_float(ap_float &&) = default;
  ap_float(const ap_float &) = default;
  ap_float& operator=(ap_float &&) = default;
  ap_float& operator=(const ap_float &) = default;

  // Returns a reference to the sign bit
  INLINE NODEBUG sign_ref_t sign_ref() noexcept { return bits[W-1]; }
  INLINE NODEBUG bool sign_ref() const noexcept { return bits[W-1]; }

  // Returns a reference to the exponent bits range (biased value)
  INLINE NODEBUG exponent_ref_t exponent_ref() noexcept { return bits.range(W-2, W-E-1); }
  INLINE NODEBUG const exponent_ref_t exponent_ref() const noexcept { return bits.range(W-2, W-E-1); }

  // Returns a reference to the mantissa bits range (doesn't include sign)
  INLINE NODEBUG mantissa_ref_t mantissa_ref() noexcept { return bits.range(W-E-2, 0); }
  INLINE NODEBUG const mantissa_ref_t mantissa_ref() const noexcept { return bits.range(W-E-2, 0); }

  // Bits and pieces constructor (biased exponent)
  INLINE NODEBUG
  ap_float(sign_t s, exponent_t e, mantissa_t m) noexcept {
    sign_ref() = s;
    exponent_ref() = e;
    mantissa_ref() = m;
  }

  INLINE NODEBUG
  ap_float(sign_ref_t s, exponent_ref_t e, mantissa_ref_t m) noexcept {
    sign_ref() = s;
    exponent_ref() = e;
    mantissa_ref() = m;
  }

  // Exponent special values
  INLINE NODEBUG static constexpr exponent_t exponent_denorm() noexcept { return 0; }
  INLINE NODEBUG static constexpr exponent_t exponent_min() noexcept { return 1; }
  INLINE NODEBUG static constexpr exponent_t exponent_max() noexcept { return -2; }
  INLINE NODEBUG static constexpr exponent_t exponent_infinity() noexcept { return -1; }

  // Mantissa special values
  INLINE NODEBUG
  static constexpr mantissa_t mantissa_quiet_mask() noexcept {
    return ((mantissa_t)-1) << (M-2);
  }

  // Exponent bias
  INLINE NODEBUG
  static constexpr int exponent_bias() noexcept {
    return (exponent_t(1) << (E-1)) - 1;
  }

  // Special values
  INLINE NODEBUG
  static constexpr ap_float lowest() noexcept {
    return ap_float(true, exponent_max(), -1);
  }
  INLINE NODEBUG
  static constexpr ap_float zero() noexcept {
    return ap_float(false, exponent_denorm(), 0);
  }
  INLINE NODEBUG
  static constexpr ap_float min_denorm() noexcept {
    return ap_float(false, exponent_denorm(), 1);
  }
  INLINE NODEBUG
  static constexpr ap_float min() noexcept {
    return ap_float(false, exponent_min(), 0);
  }
  INLINE NODEBUG
  static constexpr ap_float epsilon() noexcept {
    return ap_float(false, exponent_bias() - (M-1), 0);
  }
  INLINE NODEBUG
  static constexpr ap_float one() noexcept {
    return ap_float(false, exponent_bias(), 0);
  }
  INLINE NODEBUG
  static constexpr ap_float one_half() noexcept {
    return ap_float(false, exponent_bias() - 1, 0);
  }
  INLINE NODEBUG
  static constexpr ap_float max() noexcept {
    return ap_float(false, exponent_max(), -1);
  }
  INLINE NODEBUG
  static constexpr ap_float infinity() noexcept {
    return ap_float(false, exponent_infinity(), 0);
  }
  INLINE NODEBUG
  static constexpr ap_float quiet_NaN() noexcept {
    return ap_float(false, exponent_infinity(), mantissa_quiet_mask());
  }
  INLINE NODEBUG
  static constexpr ap_float signaling_NaN() noexcept {
    return ap_float(false, exponent_infinity(), mantissa_quiet_mask() >> 1);
  }

  // Friend with other template specialization of itself (to allow conversions)
  template<int W2, int E2>
  friend class ap_float;

  // Get exponent value (unbiased value, only valid for normals)
  INLINE NODEBUG
  int get_unbiased_exponent() const noexcept {
    int biased = exponent_ref();
    assert(biased > exponent_denorm() && "Unbiased exponent on denormals is unsupported");
    assert(biased < exponent_infinity() && "Unbiased exponent on infinite/NaN is unsupported");
    return biased - exponent_bias();
  }

  // Set exponent value (unbiased value, only valid for normals)
  INLINE NODEBUG
  void set_unbiased_exponent(int e) noexcept {
    int biased = e + exponent_bias();
    assert(biased > exponent_denorm() && "Unbiased exponent on denormals is unsupported");
    assert(biased < exponent_infinity() && "Unbiased exponent on infinite/NaN is unsupported");
    exponent_ref() = biased;
  }

  // Conversion from native floating point types
  INLINE NODEBUG
  explicit ap_float(const half &h) noexcept {
    static_assert(W == 16 && E == 5, "conversion from half only supported on ap_float<16, 5>");
#ifndef __SYNTHESIS__
    xip_fpo_t val;
    this->init_xip_fpo_t(val);
    xip_fpo_set_flt(val, (float)h);
    this->from_xip_fpo_t(val);
    this->fini_xip_fpo_t(val);
#else
    bits = (const decltype(bits) &)h;
#endif
  }

  INLINE NODEBUG
  explicit ap_float(const float &f) noexcept {
    static_assert(W == 32 && E == 8, "conversion from float only supported on ap_float<32, 8>");
#ifndef __SYNTHESIS__
    xip_fpo_t val;
    this->init_xip_fpo_t(val);
    xip_fpo_set_flt(val, f);
    this->from_xip_fpo_t(val);
    this->fini_xip_fpo_t(val);
#else
    bits = (const decltype(bits) &)f;
#endif
  }

  INLINE NODEBUG
  explicit ap_float(const double &d) noexcept {
    static_assert(W == 64 && E == 11, "conversion from double only supported on ap_float<64, 11>");
#ifndef __SYNTHESIS__
    xip_fpo_t val;
    this->init_xip_fpo_t(val);
    xip_fpo_set_d(val, d);
    this->from_xip_fpo_t(val);
    this->fini_xip_fpo_t(val);
#else
    bits = (const decltype(bits) &)d;
#endif
  }

  // Conversion to native floating point types
  INLINE NODEBUG
  explicit operator half() const noexcept {
    static_assert(W == 16 && E == 5, "conversion to half only supported on ap_float<16, 5>");
    return *((const half *)this);
  }

  INLINE NODEBUG
  explicit operator float() const noexcept {
    static_assert(W == 32 && E == 8, "conversion from float only supported on ap_float<32, 8>");
    return *((const float *)this);
  }

  INLINE NODEBUG
  explicit operator double() const noexcept {
    static_assert(W == 64 && E == 11, "conversion from double only supported on ap_float<64, 11>");
    return *((const double *)this);
  }

  // Conversion from ap_fixed
  template<int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
  INLINE NODEBUG
  explicit ap_float(const ap_fixed<W2, I2, Q2, O2, N2> &other) noexcept {
    static_assert(E >= log2_ceil(W2+3)+1,
                  "conversion from ap_fixed only supported on ap_float with"
                  " sufficient exponent bitwidth");
#ifdef __SYNTHESIS__
    __fpga_float_from_fixed(&bits, &other, W2, I2, W, E);
#else
    xip_fpo_t out;
    int base = 2;
    std::string str;

    this->init_xip_fpo_t(out);

    str = other.to_string(base , true); // example : -3.75 -> -11.11
    const char* ch = str.c_str();
    xip_fpo_set_str(out, ch, base);
    this->from_xip_fpo_t(out);
  
    this->fini_xip_fpo_t(out);
#endif
  }

  // Conversion to ap_fixed
  template<int W2, int I2, ap_q_mode Q2, ap_o_mode O2, int N2>
  INLINE NODEBUG
  explicit operator ap_fixed<W2, I2, Q2, O2, N2>() const noexcept {
    static_assert(E >= log2_ceil(W2+3)+1,
                  "conversion from ap_fixed only supported on ap_float with"
                  " sufficient exponent bitwidth");
#ifdef __SYNTHESIS__
    ap_fixed<W2, I2, Q2, O2, N2> res;
    __fpga_float_to_fixed(&res, this, W, E, W2, I2);
    return res;
#else
    ap_fixed<W2, I2, Q2, O2, N2> res;
    xip_fpo_t in;
    xip_fpo_fix_t out;
    xip_fpo_prec_t i = I2;
    xip_fpo_prec_t frac = W2 - I2;
    int base = 2;
    char* ch = nullptr;

    this->init_xip_fpo_t(in);
    this->to_xip_fpo_t(in);
    xip_fpo_fix_init2(out, i, frac);
    
    xip_fpo_flttofix(out,in);
    ch = xip_fpo_fix_get_str(ch, base , out); // example : -3.75 -> -11.11
    res = ap_fixed<W2, I2, Q2, O2, N2>(ch, base); // if base = 10 this line is: res = ch
    
    this->fini_xip_fpo_t(in);
    xip_fpo_fix_clear(out);
    xip_fpo_free_str(ch);
    
    return res;
  #endif
  }

  // Conversions from/to other bitwidth
  template<int W2, int E2>
  INLINE NODEBUG
  ap_float(const ap_float<W2, E2> &other) noexcept {
#ifdef __SYNTHESIS__
    __fpga_float_to_float(this, &other, W2, E2, W, E);
#else
    xip_fpo_t in;
    xip_fpo_t out;
    other.init_xip_fpo_t(in);
    this->init_xip_fpo_t(out);

    other.to_xip_fpo_t(in);
    xip_fpo_flttoflt(out, in);
    this->from_xip_fpo_t(out);

    other.fini_xip_fpo_t(in);
    this->fini_xip_fpo_t(out);
#endif
  }

  // Conversion from integer types
#define AP_FLOAT_FROM_INT(T)                                                   \
  INLINE NODEBUG                                                               \
  explicit ap_float(signed T i) noexcept : ap_float(ap_fixed<W,W>(i)) {}       \
  INLINE NODEBUG                                                               \
  explicit ap_float(unsigned T i) noexcept : ap_float(ap_fixed<W,W>(i)) {}
  AP_FLOAT_FROM_INT(char)
  AP_FLOAT_FROM_INT(short)
  AP_FLOAT_FROM_INT(int)
  AP_FLOAT_FROM_INT(long)
  AP_FLOAT_FROM_INT(long long)
#undef AP_FLOAT_FROM_INT

  // Conversion to integer types
#define AP_FLOAT_TO_INT(T)                                                     \
  INLINE NODEBUG                                                               \
  explicit operator signed T() const noexcept {                                \
    return (T) (ap_fixed<W,W>) *this;                                          \
  }                                                                            \
  INLINE NODEBUG                                                               \
  explicit operator unsigned T() const noexcept {                              \
    return (T) (ap_fixed<W,W>) *this;                                          \
  }
  AP_FLOAT_TO_INT(char)
  AP_FLOAT_TO_INT(short)
  AP_FLOAT_TO_INT(int)
  AP_FLOAT_TO_INT(long)
  AP_FLOAT_TO_INT(long long)
#undef AP_FLOAT_TO_INT

  // Inplace addition
  INLINE NODEBUG
  ap_float<W, E>& operator+=(const ap_float<W, E> &other) noexcept {
#ifdef __SYNTHESIS__
    __fpga_float_add(this, this, &other, W, E);
#else
    xip_fpo_t lhs;
    xip_fpo_t rhs;
    xip_fpo_t out;
    this->init_xip_fpo_t(lhs);
    other.init_xip_fpo_t(rhs);
    this->init_xip_fpo_t(out);

    this->to_xip_fpo_t(lhs);
    other.to_xip_fpo_t(rhs);
    xip_fpo_add(out, lhs, rhs);
    this->from_xip_fpo_t(out);

    this->fini_xip_fpo_t(lhs);
    other.fini_xip_fpo_t(rhs);
    this->fini_xip_fpo_t(out);
#endif
    return *this;
  }

  // Inplace subtraction
  INLINE NODEBUG
  ap_float<W, E>& operator-=(const ap_float<W, E> &other) noexcept {
#ifdef __SYNTHESIS__
    __fpga_float_sub(this, this, &other, W, E);
#else
    xip_fpo_t lhs;
    xip_fpo_t rhs;
    xip_fpo_t out;
    this->init_xip_fpo_t(lhs);
    other.init_xip_fpo_t(rhs);
    this->init_xip_fpo_t(out);

    this->to_xip_fpo_t(lhs);
    other.to_xip_fpo_t(rhs);
    xip_fpo_sub(out, lhs, rhs);
    this->from_xip_fpo_t(out);

    this->fini_xip_fpo_t(lhs);
    other.fini_xip_fpo_t(rhs);
    this->fini_xip_fpo_t(out);
#endif
    return *this;
  }

  // Inplace multiplication
  INLINE NODEBUG
  ap_float<W, E>& operator*=(const ap_float<W, E> &other) noexcept {
#ifdef __SYNTHESIS__
    __fpga_float_mul(this, this, &other, W, E);
#else
    xip_fpo_t lhs;
    xip_fpo_t rhs;
    xip_fpo_t out;
    this->init_xip_fpo_t(lhs);
    other.init_xip_fpo_t(rhs);
    this->init_xip_fpo_t(out);

    this->to_xip_fpo_t(lhs);
    other.to_xip_fpo_t(rhs);
    xip_fpo_mul(out, lhs, rhs);
    this->from_xip_fpo_t(out);

    this->fini_xip_fpo_t(lhs);
    other.fini_xip_fpo_t(rhs);
    this->fini_xip_fpo_t(out);
#endif
    return *this;
  }

  // Inplace division
  INLINE NODEBUG
  ap_float<W, E>& operator/=(const ap_float<W, E> &other) noexcept {
#ifdef __SYNTHESIS__
    __fpga_float_div(this, this, &other, W, E);
#else
    xip_fpo_t lhs;
    xip_fpo_t rhs;
    xip_fpo_t out;
    this->init_xip_fpo_t(lhs);
    other.init_xip_fpo_t(rhs);
    this->init_xip_fpo_t(out);

    this->to_xip_fpo_t(lhs);
    other.to_xip_fpo_t(rhs);
    xip_fpo_div(out, lhs, rhs);
    this->from_xip_fpo_t(out);

    this->fini_xip_fpo_t(lhs);
    other.fini_xip_fpo_t(rhs);
    this->fini_xip_fpo_t(out);
#endif
    return *this;
  }

  // Inplace pre-increment
  INLINE NODEBUG
  ap_float& operator++() noexcept {
    *this += one();
    return *this;
  }

  // Inplace post-increment
  INLINE NODEBUG
  ap_float operator++(int) noexcept {
    ap_float res = *this;
    ++*this;
    return res;
  }

  // Inplace pre-decrement
  INLINE NODEBUG
  ap_float& operator--() noexcept {
    *this -= one();
    return *this;
  }

  // Inplace post-decrement
  INLINE NODEBUG
  ap_float operator--(int) noexcept {
    ap_float res = *this;
    --*this;
    return res;
  }

  // Comparisons
  template<int W0, int E0>
  INLINE NODEBUG
  friend bool operator==(const ap_float<W0, E0> &lhs,
                         const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend bool operator!=(const ap_float<W0, E0> &lhs,
                         const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend bool operator<(const ap_float<W0, E0> &lhs,
                        const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend bool operator>(const ap_float<W0, E0> &lhs,
                        const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend bool operator<=(const ap_float<W0, E0> &lhs,
                         const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend bool operator>=(const ap_float<W0, E0> &lhs,
                         const ap_float<W0, E0> &rhs) noexcept;

  // Unary Expression Operator Overloading
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> operator+(ap_float<W0, E0> val) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> operator-(ap_float<W0, E0> val) noexcept;

  // Binary Expression Operator Overloading
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> operator+(ap_float<W0, E0> lhs,
                                    const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> operator-(ap_float<W0, E0> lhs,
                                    const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> operator*(ap_float<W0, E0> lhs,
                                    const ap_float<W0, E0> &rhs) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> operator/(ap_float<W0, E0> lhs,
                                    const ap_float<W0, E0> &rhs) noexcept;

  // Math library
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> hls::fma(const ap_float<W0, E0> &lhs,
                                   const ap_float<W0, E0> &rhs,
                                   const ap_float<W0, E0> &add) noexcept;
  template<int W0, int E0>
  INLINE NODEBUG
  friend ap_float<W0, E0> hls::sqrt(const ap_float<W0, E0> &val) noexcept;

#ifndef __SYNTHESIS__
  // Output stream printing
  template<int W0, int E0>
  INLINE NODEBUG
  friend std::ostream &operator<<(std::ostream &os,
                                  const ap_float<W0, E0> &val) noexcept;
  // Input stream parsing
  template<int W0, int E0>
  INLINE NODEBUG
  friend std::istream &operator>>(std::istream &os,
                                  ap_float<W0, E0> &val) noexcept;
#endif
};

template<int W, int E>
INLINE NODEBUG
bool operator==(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs) noexcept {
#ifdef __SYNTHESIS__
  return __fpga_float_compare_eq(&lhs, &rhs, W, E);
#else
  xip_fpo_t xlhs;
  xip_fpo_t xrhs;
  int res;
  lhs.init_xip_fpo_t(xlhs);
  rhs.init_xip_fpo_t(xrhs);

  lhs.to_xip_fpo_t(xlhs);
  rhs.to_xip_fpo_t(xrhs);
  xip_fpo_equal(&res, xlhs, xrhs);

  lhs.fini_xip_fpo_t(xlhs);
  rhs.fini_xip_fpo_t(xrhs);
  return res;
#endif
}

template<int W, int E>
INLINE NODEBUG
bool operator!=(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs) noexcept {
#ifdef __SYNTHESIS__
  return __fpga_float_compare_ne(&lhs, &rhs, W, E);
#else
  xip_fpo_t xlhs;
  xip_fpo_t xrhs;
  int res;
  lhs.init_xip_fpo_t(xlhs);
  rhs.init_xip_fpo_t(xrhs);

  lhs.to_xip_fpo_t(xlhs);
  rhs.to_xip_fpo_t(xrhs);
  xip_fpo_notequal(&res, xlhs, xrhs);

  lhs.fini_xip_fpo_t(xlhs);
  rhs.fini_xip_fpo_t(xrhs);
  return res;
#endif
}

template<int W, int E>
INLINE NODEBUG
bool operator<(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs) noexcept {
#ifdef __SYNTHESIS__
  return __fpga_float_compare_lt(&lhs, &rhs, W, E);
#else
  xip_fpo_t xlhs;
  xip_fpo_t xrhs;
  int res;
  lhs.init_xip_fpo_t(xlhs);
  rhs.init_xip_fpo_t(xrhs);

  lhs.to_xip_fpo_t(xlhs);
  rhs.to_xip_fpo_t(xrhs);
  xip_fpo_less(&res, xlhs, xrhs);

  lhs.fini_xip_fpo_t(xlhs);
  rhs.fini_xip_fpo_t(xrhs);
  return res;
#endif
}

template<int W, int E>
INLINE NODEBUG
bool operator>(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs) noexcept {
  return rhs < lhs;
}

template<int W, int E>
INLINE NODEBUG
bool operator<=(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs) noexcept {
#ifdef __SYNTHESIS__
  return __fpga_float_compare_le(&lhs, &rhs, W, E);
#else
  xip_fpo_t xlhs;
  xip_fpo_t xrhs;
  int res;
  lhs.init_xip_fpo_t(xlhs);
  rhs.init_xip_fpo_t(xrhs);

  lhs.to_xip_fpo_t(xlhs);
  rhs.to_xip_fpo_t(xrhs);
  xip_fpo_lessequal(&res, xlhs, xrhs);

  lhs.fini_xip_fpo_t(xlhs);
  rhs.fini_xip_fpo_t(xrhs);
  return res;
#endif
}

template<int W, int E>
INLINE NODEBUG
bool operator>=(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs) noexcept {
  return rhs <= lhs;
}

// Expression unary plus
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> operator+(ap_float<W, E> val) noexcept {
  return val;
}

// Expression unary minus
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> operator-(ap_float<W, E> val) noexcept {
  val.sign_ref() = ~val.sign_ref();
  return val;
}

// Expression addition
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> operator+(ap_float<W, E> lhs,
                         const ap_float<W, E> &rhs) noexcept {
  return lhs += rhs;
}

// Expression subtraction
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> operator-(ap_float<W, E> lhs,
                         const ap_float<W, E> &rhs) noexcept {
  return lhs -= rhs;
}

// Expression multiplication
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> operator*(ap_float<W, E> lhs,
                         const ap_float<W, E> &rhs) noexcept {
  return lhs *= rhs;
}

// Expression division
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> operator/(ap_float<W, E> lhs,
                         const ap_float<W, E> &rhs) noexcept {
  return lhs /= rhs;
}

#ifndef __SYNTHESIS__
// Output stream printing
template<int W, int E>
std::ostream &operator<<(std::ostream &os, const ap_float<W, E> &val) noexcept {
  double wr;
  wr = val.to_double(); // FIXME Raw double conversion display too many digits
  os << wr;
  return os;
}

// Input stream parsing
template<int W, int E>
std::istream &operator>>(std::istream &os, ap_float<W, E> &val) noexcept {
  double rd;
  xip_fpo_t xval;
  
  os >> rd;

  val.init_xip_fpo_t(xval); // inizialize the xip_fpo_t
  xip_fpo_set_d(xval,rd);   // set xval to the double value from rd (rounding performed)
  val.from_xip_fpo_t(xval); // get double from xip -> double to ap_float (no rounding required)
  
  val.fini_xip_fpo_t(xval);
  return os;
}
#endif

namespace hls {
// Fused Multiply and Add
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> fma(const ap_float<W, E> &lhs, const ap_float<W, E> &rhs,
                   const ap_float<W, E> &add) noexcept {
  ap_float<W, E> res;
#ifdef __SYNTHESIS__
  __fpga_float_fma(&res, &lhs, &rhs, &add, W, E);
#else
  xip_fpo_t xlhs;
  xip_fpo_t xrhs;
  xip_fpo_t xadd;
  xip_fpo_t xres;
  lhs.init_xip_fpo_t(xlhs);
  rhs.init_xip_fpo_t(xrhs);
  add.init_xip_fpo_t(xadd);
  res.init_xip_fpo_t(xres);

  lhs.to_xip_fpo_t(xlhs);
  rhs.to_xip_fpo_t(xrhs);
  add.to_xip_fpo_t(xadd);
  xip_fpo_fma(xres, xlhs, xrhs, xadd);
  res.from_xip_fpo_t(xres);

  lhs.fini_xip_fpo_t(xlhs);
  rhs.fini_xip_fpo_t(xrhs);
  add.fini_xip_fpo_t(xadd);
  res.fini_xip_fpo_t(xres);
#endif
  return res;
}

// Square Root
template<int W, int E>
INLINE NODEBUG
ap_float<W, E> sqrt(const ap_float<W, E> &val) noexcept {
  ap_float<W, E> res;
#ifdef __SYNTHESIS__
  __fpga_float_sqrt(&res, &val, W, E);
#else
  xip_fpo_t xval;
  xip_fpo_t xres;
  val.init_xip_fpo_t(xval);
  res.init_xip_fpo_t(xres);

  val.to_xip_fpo_t(xval);
  xip_fpo_sqrt(xres, xval);
  res.from_xip_fpo_t(xres);

  val.fini_xip_fpo_t(xval);
  res.fini_xip_fpo_t(xres);
#endif
  return res;
}
} // namespace hls

// Shorthand for native type equivalents
using ap_float_half = ap_float<16, 5>;
using ap_float_single = ap_float<32, 8>;
using ap_float_double = ap_float<64, 11>;

namespace std {
template<int W, int E>
struct numeric_limits<ap_float<W, E>> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;

  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;

  static constexpr float_denorm_style has_denorm = denorm_present;
  static constexpr bool has_denorm_loss = true;
  static constexpr float_round_style round_style = round_to_nearest;

  static constexpr bool is_iec559 = has_infinity && has_quiet_NaN &&
                                    has_signaling_NaN;

  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = false;

  static constexpr int digits = W - E;
  // log10(2) = 0.301029995663981  (D10317)
  static constexpr int digits10 = static_cast<int>((digits - 1) * 0.301029995663981);
  static constexpr int max_digits10 = static_cast<int>(digits * 0.301029995663981 + 2);
  
  static constexpr int radix = 2;
  static constexpr int min_exponent =  3 - ( 1 << (E - 1));
  static constexpr int min_exponent10 = 0; // FIXME
  static constexpr int max_exponent = (1 << (E - 1));
  static constexpr int max_exponent10 = 0; // FIXME

  static constexpr bool traps = false;
  static constexpr bool tinyness_before = false;

  INLINE NODEBUG
  static constexpr ap_float<W, E> lowest() noexcept {
    return ap_float<W, E>::lowest();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> denorm_min() noexcept {
    return ap_float<W, E>::min_denorm();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> min() noexcept {
    return ap_float<W, E>::min();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> epsilon() noexcept {
    return ap_float<W, E>::epsilon();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> round_error() noexcept {
    return ap_float<W, E>::one_half();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> max() noexcept {
    return ap_float<W, E>::max();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> infinity() noexcept {
    return ap_float<W, E>::infinity();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> quiet_NaN() noexcept {
    return ap_float<W, E>::quiet_NaN();
  }

  INLINE NODEBUG
  static constexpr ap_float<W, E> signaling_NaN() noexcept {
    return ap_float<W, E>::signaling_NaN();
  }
};
} // namespace std

#endif
