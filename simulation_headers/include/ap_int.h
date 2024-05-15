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


#ifndef __AP_INT_H__
#define __AP_INT_H__


#include <etc/ap_common.h>
#include <etc/ap_int_base.h>
#include <etc/ap_int_ref.h>

//---------------------------------------------------------------

/// Sign Arbitrary Precision Type.
template <int _AP_W>
struct ap_int : ap_int_base<_AP_W, true> {
  typedef ap_int_base<_AP_W, true> Base;
  // Constructor
  INLINE NODEBUG ap_int() = default; //The default constructor is explicitly stated.
  template <int _AP_W2>
  HLS_CONSTEXPR INLINE NODEBUG ap_int(const ap_int<_AP_W2>& op): Base((ap_int_base<_AP_W2, true>)op){
#ifdef HLS_CONSTEXPR_ENABLE
    op.checkOverflowCsim(_AP_W, false);
#else
    op.checkOverflowCsim(_AP_W, true);
#endif
    Base::V = op.V;
  }

  template <int _AP_W2>
  INLINE NODEBUG ap_int(const volatile ap_int<_AP_W2>& op) {
    const_cast<const ap_int<_AP_W2>& >(op).checkOverflowCsim(_AP_W, true);
    Base::V = op.V;
  }

  template <int _AP_W2>
  HLS_CONSTEXPR INLINE NODEBUG ap_int(const ap_uint<_AP_W2>& op): Base((ap_int_base<_AP_W2, false>)op){
#ifdef HLS_CONSTEXPR_ENABLE
    op.checkOverflowCsim(_AP_W, false);
#else
    op.checkOverflowCsim(_AP_W, true);
#endif
    Base::V = op.V;
  }

  template <int _AP_W2>
  INLINE NODEBUG ap_int(const volatile ap_uint<_AP_W2>& op) {
    const_cast<const ap_uint<_AP_W2>& >(op).checkOverflowCsim(_AP_W, true);
    Base::V = op.V;
  }

#ifdef __SYNTHESIS__
#if ((__clang_major__ != 3) || (__clang_minor__ != 1))
  /// ctor from raw val
  HLS_CONSTEXPR INLINE NODEBUG ap_int(unsigned V __attribute__((bitwidth(_AP_W))), bool raw): Base(0) {
    Base::V = V;
    _AP_UNUSED_PARAM(raw);
  }
#endif
#endif

  template <int _AP_W2, bool _AP_S2>
  INLINE NODEBUG ap_int(const ap_range_ref<_AP_W2, _AP_S2>& ref) : Base(ref) {}

  template <int _AP_W2, bool _AP_S2>
  INLINE NODEBUG ap_int(const ap_bit_ref<_AP_W2, _AP_S2>& ref) : Base(ref) {}

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE NODEBUG ap_int(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& ref)
      : Base(ref) {}

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_int(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>)op) {}

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_int(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>)op) {
  }

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_int(
      const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>)op) {}

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_int(
      const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>)op) {
  }

  template <int _AP_W2, bool _AP_S2>
  HLS_CONSTEXPR INLINE NODEBUG ap_int(const ap_int_base<_AP_W2, _AP_S2>& op):Base(op) {
    //Base::V = op.V;
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  HLS_CONSTEXPR INLINE NODEBUG ap_int(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  HLS_CONSTEXPR INLINE NODEBUG ap_int(
      const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE NODEBUG ap_int(
      const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

#define CTOR(TYPE) \
  HLS_CONSTEXPR INLINE NODEBUG ap_int(TYPE val):Base(val){         \
    this->checkOverflowBaseC(val);  \
  }
   // Base::V = val; }

  CTOR(bool)
  CTOR(char)
  CTOR(signed char)
  CTOR(unsigned char)
  CTOR(short)
  CTOR(unsigned short)
  CTOR(int)
  CTOR(unsigned int)
  CTOR(long)
  CTOR(unsigned long)
  CTOR(ap_slong)
  CTOR(ap_ulong)
#undef CTOR
  HLS_CONSTEXPR ap_int(double val) : Base(val) {}
  HLS_CONSTEXPR ap_int(float val) : Base(val) {}
  ap_int(half val) : Base(val) {}

  // ap_int_base will guess radix if radix is not provided.
  INLINE NODEBUG ap_int(const char* s) : Base(s) {}

  INLINE NODEBUG ap_int(const char* s, signed char rd) : Base(s, rd) {}

  // Assignment
  /* ctor will be used when right is not of proper type. */
#ifndef _MSC_VER
  ap_int &operator=(const ap_int<_AP_W> &op2) = default;
#else
  HLS_CONSTEXPR INLINE NODEBUG ap_int &operator=(const ap_int<_AP_W> &op2) {
    Base::V = op2.V;
    return *this;
  }
#endif

  /* cannot bind volatile reference to non-volatile type. */
  INLINE NODEBUG ap_int& operator=(const volatile ap_int<_AP_W>& op2) {
    Base::V = op2.V;
    return *this;
  }

  /* cannot return volatile *this. */
  INLINE NODEBUG void operator=(const ap_int<_AP_W>& op2) volatile { 
    *((volatile typename Base::Base::DataType *)(&(Base::V))) = op2.V; 
  }

  INLINE NODEBUG void operator=(const volatile ap_int<_AP_W>& op2) volatile {
    *((volatile typename Base::Base::DataType *)(&(Base::V))) = op2.V; 
  }

}; // struct ap_int.

//---------------------------------------------------------------

/// Unsigned Arbitrary Precision Type.
template <int _AP_W>
struct ap_uint : ap_int_base<_AP_W, false> {
  typedef ap_int_base<_AP_W, false> Base;
  // Constructor
  INLINE NODEBUG ap_uint() = default; //The default constructor is explicitly stated.
  template <int _AP_W2>
  HLS_CONSTEXPR INLINE NODEBUG ap_uint(const ap_uint<_AP_W2>& op):Base((ap_int_base<_AP_W2, false>)op) {
    op.checkOverflowCsim(_AP_W, false);
    //Base::V = op.V;
  }

  template <int _AP_W2>
  HLS_CONSTEXPR INLINE NODEBUG ap_uint(const ap_int<_AP_W2>& op):Base((ap_int_base<_AP_W2, true>)op) {
    op.checkOverflowCsim(_AP_W, false);
    //Base::V = op.V;
  }

  template <int _AP_W2>
  INLINE NODEBUG ap_uint(const volatile ap_uint<_AP_W2>& op) {
    const_cast<const ap_uint<_AP_W2>& >(op).checkOverflowCsim(_AP_W, false);
    Base::V = op.V;
  }

  template <int _AP_W2>
  INLINE NODEBUG ap_uint(const volatile ap_int<_AP_W2>& op) {
    const_cast<const ap_int<_AP_W2>& >(op).checkOverflowCsim(_AP_W, false);
    Base::V = op.V;
  }

#ifdef __SYNTHESIS__
#if ((__clang_major__ != 3) || (__clang_minor__ != 1))
  /// ctor from raw val
  HLS_CONSTEXPR INLINE NODEBUG ap_uint(unsigned V __attribute__((bitwidth(_AP_W))), bool raw):Base(0) {
    Base::V = V;
    _AP_UNUSED_PARAM(raw);
  }
#endif
#endif

  template <int _AP_W2, bool _AP_S2>
  INLINE NODEBUG ap_uint(const ap_range_ref<_AP_W2, _AP_S2>& ref) : Base(ref) {}

  template <int _AP_W2, bool _AP_S2>
  INLINE NODEBUG ap_uint(const ap_bit_ref<_AP_W2, _AP_S2>& ref) : Base(ref) {}

  template <int _AP_W2, typename _AP_T2, int _AP_W3, typename _AP_T3>
  INLINE NODEBUG ap_uint(const ap_concat_ref<_AP_W2, _AP_T2, _AP_W3, _AP_T3>& ref)
      : Base(ref) {}

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_uint(const ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>)op) {}

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_uint(const ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>)op) {
  }

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_uint(
      const volatile ap_fixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, true, _AP_Q2, _AP_O2, _AP_N2>)op) {}

  template <int _AP_W2, int _AP_I2, ap_q_mode _AP_Q2, ap_o_mode _AP_O2,
            int _AP_N2>
  INLINE NODEBUG ap_uint(
      const volatile ap_ufixed<_AP_W2, _AP_I2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base((ap_fixed_base<_AP_W2, _AP_I2, false, _AP_Q2, _AP_O2, _AP_N2>)op) {
  }

  template <int _AP_W2, bool _AP_S2>
  HLS_CONSTEXPR INLINE NODEBUG ap_uint(const ap_int_base<_AP_W2, _AP_S2>& op):Base(op) {
    //Base::V = op.V;
  }

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE NODEBUG ap_uint(
      const af_bit_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE NODEBUG ap_uint(
      const af_range_ref<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

  template <int _AP_W2, int _AP_I2, bool _AP_S2, ap_q_mode _AP_Q2,
            ap_o_mode _AP_O2, int _AP_N2>
  INLINE NODEBUG ap_uint(
      const ap_fixed_base<_AP_W2, _AP_I2, _AP_S2, _AP_Q2, _AP_O2, _AP_N2>& op)
      : Base(op) {}

#define CTOR(TYPE) \
  HLS_CONSTEXPR INLINE NODEBUG ap_uint(TYPE val):Base(val) {        \
    this->checkOverflowBaseC(val);  \
  }
   // Base::V = val; }
  CTOR(bool)
  CTOR(char)
  CTOR(signed char)
  CTOR(unsigned char)
  CTOR(short)
  CTOR(unsigned short)
  CTOR(int)
  CTOR(unsigned int)
  CTOR(long)
  CTOR(unsigned long)
  CTOR(ap_slong)
  CTOR(ap_ulong)
#undef CTOR
  HLS_CONSTEXPR ap_uint(double val) : Base(val) {}
  HLS_CONSTEXPR ap_uint(float val) : Base(val) {}
  ap_uint(half val) : Base(val) {}

  // ap_int_base will guess radix if radix is not provided.
  INLINE NODEBUG ap_uint(const char* s) : Base(s) {}

  INLINE NODEBUG ap_uint(const char* s, signed char rd) : Base(s, rd) {}

  // Assignment
  /* XXX ctor will be used when right is not of proper type. */

#ifndef _MSC_VER
  ap_uint &operator=(const ap_uint<_AP_W> &op2) = default;
#else
  HLS_CONSTEXPR INLINE NODEBUG ap_uint& operator=(const ap_uint<_AP_W>& op2) {
    Base::V = op2.V;
    return *this;
  }
#endif


  /* cannot bind volatile reference to non-volatile type. */
  INLINE NODEBUG ap_uint& operator=(const volatile ap_uint<_AP_W>& op2) {
    Base::V = op2.V;
    return *this;
  }

  /* cannot return volatile *this. */
  INLINE NODEBUG void operator=(const ap_uint<_AP_W>& op2) volatile { 
    *((volatile typename Base::Base::DataType *)(&(Base::V))) = op2.V; 
  }

  INLINE NODEBUG void operator=(const volatile ap_uint<_AP_W>& op2) volatile {
    *((volatile typename Base::Base::DataType *)(&(Base::V))) = op2.V; 
  }

}; // struct ap_uint.

#define ap_bigint ap_int
#define ap_biguint ap_uint

#if !defined(__SYNTHESIS__) && (defined(SYSTEMC_H) || defined(SYSTEMC_INCLUDED))
// XXX sc_trace overload for ap_fixed is already included in
// "ap_sysc/ap_sc_extras.h", so do not define in synthesis.
template <int _AP_W>
INLINE void sc_trace(sc_core::sc_trace_file* tf, const ap_int<_AP_W>& op,
                     const std::string& name) {
  if (tf) tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}

template <int _AP_W>
INLINE void sc_trace(sc_core::sc_trace_file* tf, const ap_uint<_AP_W>& op,
                     const std::string& name) {
  if (tf) tf->trace(sc_dt::sc_lv<_AP_W>(op.to_string(2).c_str()), name);
}
#endif // System C sim

#include <etc/ap_int_special.h>

#endif // ifndef __AP_INT_H__ else

// FIXME user should include ap_fixed.h when using ap_fixed.
// to avoid circular inclusion, must check whether this is required by
// ap_fixed.h
#ifndef __AP_FIXED_H__
#include <ap_fixed.h>
#endif

// -*- cpp -*-
