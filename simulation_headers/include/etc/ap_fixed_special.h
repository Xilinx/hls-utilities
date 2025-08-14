// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef AP_FIXED_SPECIAL_H
#define AP_FIXED_SPECIAL_H
#ifndef __SYNTHESIS__
#include <cstdio>
#include <cstdlib>
#endif

#ifndef __AP_COMMON_H__
#error "ap_common.h is required."
#endif

// FIXME AP_AUTOCC cannot handle many standard headers, so declare instead of
// include.
// #include <complex>
namespace std {
template <typename _Tp> class complex;
}

/*
  TODO: Modernize the code using C++11/C++14
  1. constexpr
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0415r0.html
  2. move constructor
*/

namespace std {
#ifndef AP_INLINE
#define AP_INLINE inline __attribute__((always_inline))
#endif

/*
   Specialize std::complex<ap_fixed> to zero initialization ap_fixed.

   To reduce the area cost, ap_fixed is not zero initialized, just like basic
   types float or double. However, libstdc++ provides specialization for float,
   double and long double, initializing image part to 0 when not specified.

   This has become a difficulty in switching legacy code from these C types to
   ap_fixed. To ease the tranform of legacy code, we have to implement
   specialization of std::complex<> for our type.

   As ap_fixed is a template, it is impossible to specialize only the methods
   that causes default initialization of value type in std::complex<>. An
   explicit full specialization of the template class has to be done, covering
   all the member functions and operators of std::complex<> as specified
   in standard 26.2.4 and 26.2.5.
*/
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
class complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> {
public:
  typedef ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> _Tp;
  typedef _Tp value_type;

  // 26.2.4/1
  // Constructor without argument
  // Default initialize, so that in dataflow, the variable is only written once.
  AP_NODEBUG AP_INLINE complex() = default;
  AP_NODEBUG AP_INLINE ~complex() = default;
  // Constructor with ap_fixed.
  // Zero initialize image part when not specified, so that `C(1) == C(1,0)`
  AP_NODEBUG AP_INLINE complex(const _Tp &__r, const _Tp &__i = _Tp(0))
      : _M_real(__r), _M_imag(__i) {}

  // Constructor with another complex number with the same element type
  AP_NODEBUG AP_INLINE complex(const complex<_Tp> &) = default;
  // copy assignment operator
  AP_NODEBUG AP_INLINE complex<_Tp> &operator=(const complex<_Tp> &) = default;
  // Constructor with another complex number with different element type
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this = tmp2;
  }

#if __cplusplus >= 201103L
  const AP_NODEBUG _Tp &real() const { return _M_real; }
  const AP_NODEBUG _Tp &imag() const { return _M_imag; }
#else
  AP_NODEBUG _Tp &real() { return _M_real; }
  const AP_NODEBUG _Tp &real() const { return _M_real; }
  AP_NODEBUG _Tp &imag() { return _M_imag; }
  const AP_NODEBUG _Tp &imag() const { return _M_imag; }
#endif

  AP_NODEBUG void real(_Tp __val) { _M_real = __val; }

  AP_NODEBUG void imag(_Tp __val) { _M_imag = __val; }

  // Assign this complex number with ap_fixed.
  // Zero initialize image poarrt, so that `C c; c = 1; c == C(1,0);`
  AP_NODEBUG AP_INLINE complex<_Tp> &operator=(const _Tp __t) {
    complex<_Tp> tmp(__t, 0);
    *this = tmp;
    return *this;
  }

  // __rep
  AP_NODEBUG AP_INLINE constexpr complex<_Tp> __rep() const {
    return *this;
  }

  // 26.2.5/1
  // Add ap_fixed to this complex number.
  AP_NODEBUG AP_INLINE complex<_Tp> &operator+=(const _Tp &__t) {
    _M_real += __t;
    return *this;
  }

  // 26.2.5/3
  // Subtract ap_fixed from this complex number.
  AP_NODEBUG AP_INLINE complex<_Tp> &operator-=(const _Tp &__t) {
    _M_real -= __t;
    return *this;
  }

  // 26.2.5/5
  // Multiply this complex number by ap_fixed.
  AP_NODEBUG AP_INLINE complex<_Tp> &operator*=(const _Tp &__t) {
    complex<_Tp> tmp(*this);
    tmp._M_real *= __t;
    tmp._M_imag *= __t;
    *this = tmp;
    return *this;
  }

  // 26.2.5/7
  // Divide this complex number by ap_fixed.
  AP_NODEBUG AP_INLINE complex<_Tp> &operator/=(const _Tp &__t) {
    complex<_Tp> tmp(*this);
    tmp._M_real /= __t;
    tmp._M_imag /= __t;
    *this = tmp;
    return *this;
  }

  // Assign complex number to this complex number.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this = tmp2;
    return *this;
  }

  // +=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator+=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> tmp2(*this);
    tmp2._M_real += tmp.real();
    tmp2._M_imag += tmp.imag();
    *this = tmp2;
    return *this;
  }
  // 26.2.5/9
  // Add complex number to this.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator+=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this += tmp2;
    return *this;
  }

  // -=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator-=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> tmp2(*this);
    tmp2._M_real -= tmp.real();
    tmp2._M_imag -= tmp.imag();
    *this = tmp2;
    return *this;
  }
  // 26.2.5/11
  // Subtract complex number from this.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator-=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this -= tmp2;
    return *this;
  }

  // *=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator*=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> tmp2(*this);
    auto real = tmp2._M_real * tmp.real() - tmp2._M_imag * tmp.imag();
    auto imag = tmp2._M_real * tmp.imag() + tmp2._M_imag * tmp.real();
    complex<_Tp> tmp3(real, imag);
    *this = tmp3;
    return *this;
  }
  // *= with diff types
  // 26.2.5/13
  // Multiply this by complex number.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator*=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this *= tmp2;
    return *this;
  }

  // /=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator/=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> cj(tmp.real(), -tmp.imag());
    complex<_Tp> a = (*this) * cj;
    complex<_Tp> b = cj * tmp;
    auto real = a.real() / b.real();
    auto imag = a.imag() / b.real();
    complex<_Tp> tmp3(real, imag);
    *this = tmp3;
    return *this;
  }
  // /= with diff types
  // 26.2.5/15
  // Divide this by complex number.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator/=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this /= tmp2;
    return *this;
  }

private:
  _Tp _M_real;
  _Tp _M_imag;

}; // class complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> >
/*
   Specialize std::complex<ap_ufixed> to zero initialization ap_ufixed.

   To reduce the area cost, ap_ufixed is not zero initialized, just like basic
   types float or double. However, libstdc++ provides specialization for float,
   double and long double, initializing image part to 0 when not specified.

   This has become a difficulty in switching legacy code from these C types to
   ap_ufixed. To ease the tranform of legacy code, we have to implement
   specialization of std::complex<> for our type.

   As ap_ufixed is a template, it is impossible to specialize only the methods
   that causes default initialization of value type in std::complex<>. An
   explicit full specialization of the template class has to be done, covering
   all the member functions and operators of std::complex<> as specified
   in standard 26.2.4 and 26.2.5.
*/
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
class complex<ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> {
public:
  typedef ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> _Tp;
  typedef _Tp value_type;

  // 26.2.4/1
  // Constructor without argument
  // Default initialize, so that in dataflow, the variable is only written once.
  AP_NODEBUG AP_INLINE complex() = default;
  // Constructor with ap_ufixed.
  // Zero initialize image part when not specified, so that `C(1) == C(1,0)`
  AP_NODEBUG AP_INLINE complex(const _Tp &__r, const _Tp &__i = _Tp(0))
      : _M_real(__r), _M_imag(__i) {}

  // Constructor with another complex number with the same element type
  AP_NODEBUG AP_INLINE complex(const complex<_Tp> &) = default;
  // copy assignment operator
  AP_NODEBUG AP_INLINE complex<_Tp> &operator=(const complex<_Tp> &) = default;
  // Constructor with another complex number with different element type
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this = tmp2;
  }

#if __cplusplus >= 201103L
  const AP_NODEBUG _Tp &real() const { return _M_real; }
  const AP_NODEBUG _Tp &imag() const { return _M_imag; }
#else
  AP_NODEBUG _Tp &real() { return _M_real; }
  const AP_NODEBUG _Tp &real() const { return _M_real; }
  AP_NODEBUG _Tp &imag() { return _M_imag; }
  const AP_NODEBUG _Tp &imag() const { return _M_imag; }
#endif

  AP_NODEBUG void real(_Tp __val) { _M_real = __val; }

  AP_NODEBUG void imag(_Tp __val) { _M_imag = __val; }

  // Assign this complex number with ap_ufixed.
  // Zero initialize image poarrt, so that `C c; c = 1; c == C(1,0);`
  AP_NODEBUG complex<_Tp> &operator=(const _Tp __t) {
    complex<_Tp> tmp(__t, 0);
    *this = tmp;
    return *this;
  }

  // 26.2.5/1
  // Add ap_ufixed to this complex number.
  AP_NODEBUG complex<_Tp> &operator+=(const _Tp &__t) {
    _M_real += __t;
    return *this;
  }

  // 26.2.5/3
  // Subtract ap_ufixed from this complex number.
  AP_NODEBUG complex<_Tp> &operator-=(const _Tp &__t) {
    _M_real -= __t;
    return *this;
  }

  // 26.2.5/5
  // Multiply this complex number by ap_ufixed.
  AP_NODEBUG AP_INLINE complex<_Tp> &operator*=(const _Tp &__t) {
    complex<_Tp> tmp(*this);
    tmp._M_real *= __t;
    tmp._M_imag *= __t;
    *this = tmp;
    return *this;
  }

  // 26.2.5/7
  // Divide this complex number by ap_ufixed.
  AP_NODEBUG AP_INLINE complex<_Tp> &operator/=(const _Tp &__t) {
    complex<_Tp> tmp(*this);
    tmp._M_real /= __t;
    tmp._M_imag /= __t;
    *this = tmp;
    return *this;
  }

  // Assign complex number to this complex number.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this = tmp2;
    return *this;
  }

  // +=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator+=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> tmp2(*this);
    tmp2._M_real += tmp.real();
    tmp2._M_imag += tmp.imag();
    *this = tmp2;
    return *this;
  }
  // 26.2.5/9
  // Add complex number to this.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator+=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this += tmp2;
    return *this;
  }

  // -=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator-=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> tmp2(*this);
    tmp2._M_real -= tmp.real();
    tmp2._M_imag -= tmp.imag();
    *this = tmp2;
    return *this;
  }
  // 26.2.5/11
  // Subtract complex number from this.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator-=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this -= tmp2;
    return *this;
  }

  // *=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator*=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> tmp2(*this);
    auto real = tmp2._M_real * tmp.real() - tmp2._M_imag * tmp.imag();
    auto imag = tmp2._M_real * tmp.imag() + tmp2._M_imag * tmp.real();
    complex<_Tp> tmp3(real, imag);
    *this = tmp3;
    return *this;
  }
  // *= with diff types
  // 26.2.5/13
  // Multiply this by complex number.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator*=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this *= tmp2;
    return *this;
  }

  // /=
  AP_NODEBUG AP_INLINE complex<_Tp> &operator/=(const complex<_Tp> &__z) {
    complex<_Tp> tmp(__z);
    complex<_Tp> cj(tmp.real(), -tmp.imag());
    complex<_Tp> a = (*this) * cj;
    complex<_Tp> b = cj * tmp;
    auto real = a.real() / b.real();
    auto imag = a.imag() / b.real();
    complex<_Tp> tmp3(real, imag);
    *this = tmp3;
    return *this;
  }
  // /= with diff types
  // 26.2.5/15
  // Divide this by complex number.
  template <typename _Up>
  AP_NODEBUG AP_INLINE complex<_Tp> &operator/=(const complex<_Up> &__z) {
    complex<_Up> tmp(__z);
    complex<_Tp> tmp2(tmp.real(), tmp.imag());
    *this /= tmp2;
    return *this;
  }

private:
  _Tp _M_real;
  _Tp _M_imag;

}; // class complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> >

/*
   Non-member operations
   These operations are not required by standard in 26.2.6, but libstdc++
   defines them for
   float, double or long double's specialization.
*/
//===---------------------------- ap_fixed ----------------------------------===
// Compare complex number with ap_fixed.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator==(const complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__x,
           const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__y) {
  return __x.real() == __y && __x.imag() == 0;
}

// Compare ap_fixed with complex number.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator==(const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__x,
           const complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__y) {
  return __x == __y.real() && 0 == __y.imag();
}

// Compare complex number with ap_fixed.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator!=(const complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__x,
           const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__y) {
  return __x.real() != __y || __x.imag() != 0;
}

// Compare ap_fixed with complex number.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator!=(const ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__x,
           const complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__y) {
  return __x != __y.real() || 0 != __y.imag();
}

#define ap_fixed_complex complex<ap_fixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>>

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_fixed_complex operator+(const ap_fixed_complex &__x,
                                                 const ap_fixed_complex &__y) {
  ap_fixed_complex __r(__x);
  __r += __y;
  return __r;
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_fixed_complex operator-(const ap_fixed_complex &__x,
                                                 const ap_fixed_complex &__y) {
  ap_fixed_complex __r(__x);
  __r -= __y;
  return __r;
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_fixed_complex operator*(const ap_fixed_complex &__x,
                                                 const ap_fixed_complex &__y) {
  ap_fixed_complex __r(__x);
  __r *= __y;
  return __r;
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_fixed_complex operator/(const ap_fixed_complex &__x,
                                                 const ap_fixed_complex &__y) {
  ap_fixed_complex __r(__x);
  __r /= __y;
  return __r;
}

#undef ap_fixed_complex

//===---------------------------- ap_ufixed ---------------------------------===

// Compare complex number with ap_ufixed.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator==(const complex<ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__x,
           const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__y) {
  return __x.real() == __y && __x.imag() == 0;
}

// Compare ap_ufixed with complex number.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator==(const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__x,
           const complex<ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__y) {
  return __x == __y.real() && 0 == __y.imag();
}

// Compare complex number with ap_ufixed.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator!=(const complex<ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__x,
           const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__y) {
  return __x.real() != __y || __x.imag() != 0;
}

// Compare ap_ufixed with complex number.
template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG bool
operator!=(const ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N> &__x,
           const complex<ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>> &__y) {
  return __x != __y.real() || 0 != __y.imag();
}

#define ap_ufixed_complex complex<ap_ufixed<_AP_W, _AP_I, _AP_Q, _AP_O, _AP_N>>

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_ufixed_complex
operator+(const ap_ufixed_complex &__x, const ap_ufixed_complex &__y) {
  ap_ufixed_complex __r(__x);
  __r += __y;
  return __r;
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_ufixed_complex
operator-(const ap_ufixed_complex &__x, const ap_ufixed_complex &__y) {
  ap_ufixed_complex __r(__x);
  __r -= __y;
  return __r;
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_ufixed_complex
operator*(const ap_ufixed_complex &__x, const ap_ufixed_complex &__y) {
  ap_ufixed_complex __r(__x);
  __r *= __y;
  return __r;
}

template <int _AP_W, int _AP_I, ap_q_mode _AP_Q, ap_o_mode _AP_O, int _AP_N>
AP_INLINE AP_NODEBUG ap_ufixed_complex
operator/(const ap_ufixed_complex &__x, const ap_ufixed_complex &__y) {
  ap_ufixed_complex __r(__x);
  __r /= __y;
  return __r;
}

#undef ap_ufixed_complex

} // namespace std

#endif // ifndef AP_FIXED_SPECIAL_H

