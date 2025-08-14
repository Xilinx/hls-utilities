// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef __AP_FIXED_H__
#define __AP_FIXED_H__

#include <etc/ap_common.h>
#include <etc/ap_fixed_base.h>
#include <etc/ap_fixed_ref.h>

//---------------------------------------------------------------

/// Signed Arbitrary Precision Fixed-Point Type.
// default for _AP_Q, _AP_O and _AP_N set in ap_decl.h
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
struct ap_fixed : ap_fixed_base<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> {
  static_assert(_AP_W > 0, "ap_fixed bitwidth must be positive");

  typedef ap_fixed_base<_AP_W, _AP_I, true, _AP_Q, _AP_O, _AP_N> Base;
  // Constructor
  /// default ctor
  AP_INLINE AP_NODEBUG ap_fixed() = default;

  /// copy ctor from ap_fixed_base.
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  HLS_CONSTEXPR_EXTRA AP_INLINE AP_NODEBUG ap_fixed(const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2,
                                      _AP_O2, _AP_N2>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  AP_INLINE AP_NODEBUG ap_fixed(const volatile ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2,
                                               _AP_O2, _AP_N2>& op)
      : Base(op) {}

  //// from ap_fixed
  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_fixed(
  //    const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_fixed(
  //    const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

  //// from ap_ufixed.
  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_fixed(
  //    const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>(op)) {
  //}

  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_fixed(
  //    const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>(op)) {
  //}

  /// copy ctor from ap_int_base.
  template <int _AP_W2, bool _AP_S2>
  HLS_CONSTEXPR_EXTRA AP_INLINE AP_NODEBUG ap_fixed(const ap_int_base<_AP_W2, _AP_S2>& op) : Base(op) {}

  template <int _AP_W2, bool _AP_S2>
  AP_INLINE AP_NODEBUG ap_fixed(const volatile ap_int_base<_AP_W2, _AP_S2>& op) : Base(op) {}

#ifdef __SYNTHESIS__
#if ((__clang_major__ != 3) || (__clang_minor__ != 1))
  /// ctor from raw val
  AP_INLINE AP_NODEBUG ap_fixed(unsigned V __attribute__((bitwidth(_AP_W))), bool raw) {
    Base::V = V;
    _AP_UNUSED_PARAM(raw);
  }
#endif
#endif

  //// from ap_int.
  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_fixed(const ap_int<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, true>(op)) {}

  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_fixed(const volatile ap_int<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, true>(op)) {}

  //// from ap_uint.
  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_fixed(const ap_uint<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, false>(op)) {}

  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_fixed(const volatile ap_uint<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, false>(op)) {}

  // from ap_bit_ref.
  template <int _AP_W2, bool _AP_S2>
  AP_INLINE AP_NODEBUG ap_fixed(const ap_bit_ref<_AP_W2, _AP_S2>& op) : Base(op) {}

  // from ap_range_ref.
  template <int _AP_W2, bool _AP_S2>
  AP_INLINE AP_NODEBUG ap_fixed(const ap_range_ref<_AP_W2, _AP_S2>& op) : Base(op) {}

  // from ap_concat_ref.
  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  AP_INLINE AP_NODEBUG ap_fixed(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op)
      : Base(op) {}

  // from af_bit_ref.
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  AP_INLINE AP_NODEBUG ap_fixed(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

  // from af_range_ref.
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  AP_INLINE AP_NODEBUG ap_fixed(
      const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

// from c types.
#define CTOR(TYPE) \
  AP_INLINE AP_NODEBUG ap_fixed(TYPE v) : Base(v) {}

  HLS_CONSTEXPR_EXTRA CTOR(bool)
  HLS_CONSTEXPR_EXTRA CTOR(char)
  HLS_CONSTEXPR_EXTRA CTOR(signed char)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned char)
  HLS_CONSTEXPR_EXTRA CTOR(short)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned short)
  HLS_CONSTEXPR_EXTRA CTOR(int)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned int)
  HLS_CONSTEXPR_EXTRA CTOR(long)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned long)
  HLS_CONSTEXPR_EXTRA CTOR(ap_slong)
  HLS_CONSTEXPR_EXTRA CTOR(ap_ulong)
  CTOR(half)
  CTOR(float)
  CTOR(double)
#undef CTOR

  AP_INLINE AP_NODEBUG ap_fixed(const char* s) : Base(s) {}

  AP_INLINE AP_NODEBUG ap_fixed(const char* s, signed char rd) : Base(s, rd) {}

  // Assignment
  // The assignment operator is technically inherited; however, it is always
  // hidden by an explicitly or implicitly defined assignment operator for the
  // derived class.
  /* XXX ctor will be used when right is not of proper type. */
#ifndef _MSC_VER
  ap_fixed &
  operator=(const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &op) = default;
#else
  HLS_CONSTEXPR_EXTRA AP_INLINE AP_NODEBUG ap_fixed& operator=(
      const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) {
      Base::V = op.V;
      return *this;
  }
#endif

  AP_INLINE AP_NODEBUG void operator=(
      const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) volatile {
    Base::V = op.V;
  }

  AP_INLINE AP_NODEBUG ap_fixed& operator=(
      const volatile ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) {
    Base::V = op.V;
    return *this;
  }

  AP_INLINE AP_NODEBUG void operator=(
      const volatile ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) volatile {
    Base::V = op.V;
  }
}; // struct ap_fixed.

//-------------------------------------------------------------------

// Unsigned Arbitrary Precision Fixed-Point Type.
// default for _AP_Q, _AP_O and _AP_N set in ap_decl.h
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
struct ap_ufixed : ap_fixed_base<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> {
  static_assert(_AP_W > 0, "ap_ufixed bitwidth must be positive");

  typedef ap_fixed_base<_AP_W, _AP_I, false, _AP_Q, _AP_O, _AP_N> Base;
  // Constructor
  /// default ctor
  AP_INLINE AP_NODEBUG ap_ufixed() = default;

  /// copy ctor from ap_fixed_base
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  HLS_CONSTEXPR_EXTRA AP_INLINE AP_NODEBUG ap_ufixed(const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2,
                                       _AP_O2, _AP_N2>& op)
      : Base(op) {}

  /// copy ctor from ap_fixed_base
  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  AP_INLINE AP_NODEBUG ap_ufixed(const volatile ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2,
                                                _AP_O2, _AP_N2>& op)
      : Base(op) {}

  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_ufixed(
  //    const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_ufixed(
  //    const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>(op)) {}

  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_ufixed(
  //    const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>(op)) {
  //}

  //template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
  //          int _AP_N2>
  //AP_INLINE AP_NODEBUG ap_ufixed(
  //    const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
  //    : Base(ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>(op)) {
  //}

  /// copy ctor from ap_int_base.
  template <int _AP_W2, bool _AP_S2>
  HLS_CONSTEXPR_EXTRA AP_INLINE AP_NODEBUG ap_ufixed(const ap_int_base<_AP_W2, _AP_S2>& op) : Base(op) {}

  template <int _AP_W2, bool _AP_S2>
  AP_INLINE AP_NODEBUG ap_ufixed(const volatile ap_int_base<_AP_W2, _AP_S2>& op) : Base(op) {}

#ifdef __SYNTHESIS__
#if ((__clang_major__ != 3) || (__clang_minor__ != 1))
  /// ctor from  raw val
  AP_INLINE AP_NODEBUG ap_ufixed(unsigned V __attribute__((bitwidth(_AP_W))), bool raw) {
    Base::V = V;
    _AP_UNUSED_PARAM(raw);
  }
#endif
#endif

  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_ufixed(const ap_int<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, true>(op)) {}

  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_ufixed(const volatile ap_int<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, true>(op)) {}

  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_ufixed(const ap_uint<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, false>(op)) {}

  //template <int _AP_W2>
  //AP_INLINE AP_NODEBUG ap_ufixed(const volatile ap_uint<_AP_W2>& op)
  //    : Base(ap_int_base<_AP_W2, false>(op)) {}

  template <int _AP_W2, bool _AP_S2>
  AP_INLINE AP_NODEBUG ap_ufixed(const ap_bit_ref<_AP_W2, _AP_S2>& op) : Base(op) {}

  template <int _AP_W2, bool _AP_S2>
  AP_INLINE AP_NODEBUG ap_ufixed(const ap_range_ref<_AP_W2, _AP_S2>& op) : Base(op) {}

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  AP_INLINE AP_NODEBUG ap_ufixed(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  AP_INLINE AP_NODEBUG ap_ufixed(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  AP_INLINE AP_NODEBUG ap_ufixed(
      const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

#define CTOR(TYPE) \
  AP_INLINE AP_NODEBUG ap_ufixed(TYPE v) : Base(v) {}

  HLS_CONSTEXPR_EXTRA CTOR(bool)
  HLS_CONSTEXPR_EXTRA CTOR(char)
  HLS_CONSTEXPR_EXTRA CTOR(signed char)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned char)
  HLS_CONSTEXPR_EXTRA CTOR(short)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned short)
  HLS_CONSTEXPR_EXTRA CTOR(int)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned int)
  HLS_CONSTEXPR_EXTRA CTOR(long)
  HLS_CONSTEXPR_EXTRA CTOR(unsigned long)
  HLS_CONSTEXPR_EXTRA CTOR(ap_slong)
  HLS_CONSTEXPR_EXTRA CTOR(ap_ulong)
  CTOR(half)
  CTOR(float)
  CTOR(double)
#undef CTOR

  AP_INLINE AP_NODEBUG ap_ufixed(const char* s) : Base(s) {}

  AP_INLINE AP_NODEBUG ap_ufixed(const char* s, signed char rd) : Base(s, rd) {}

  // Assignment
 #ifndef _MSC_VER
  ap_ufixed &
  operator=(const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &op) = default;
#else
  HLS_CONSTEXPR_EXTRA AP_INLINE AP_NODEBUG ap_ufixed& operator=(
      const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) {
      Base::V = op.V;
      return *this;
  }
#endif

  AP_INLINE AP_NODEBUG void operator=(
      const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) volatile {
    Base::V = op.V;
  }

  AP_INLINE AP_NODEBUG ap_ufixed& operator=(
      const volatile ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op) {
    Base::V = op.V;
    return *this;
  }

  AP_INLINE AP_NODEBUG void operator=(const volatile ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O,
                                                 _AP_N>& op) volatile {
    Base::V = op.V;
  }
}; // struct ap_ufixed


#if !defined(__SYNTHESIS__) && (defined(SYSTEMC_H) || defined(SYSTEMC_INCLUDED))
// XXX sc_trace overload for ap_fixed is already included in
// "ap_sysc/ap_sc_extras.h", so do not define in synthesis.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE void sc_trace(sc_core::sc_trace_file* tf,
                     const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op,
                     const std::string& name) {
  tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE void sc_trace(sc_core::sc_trace_file* tf,
                     const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>& op,
                     const std::string& name) {
  tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}
#endif // System C sim

// Specialization of std containers, so that std::complex<ap_fixed> can have its
// image part automatically zero-initialized when only real part is provided.
#include <etc/ap_fixed_special.h>

#endif // ifndef __AP_FIXED_H__ else

// -*- cpp -*-
