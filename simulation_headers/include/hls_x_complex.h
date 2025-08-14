// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef HLS_X_COMPLEX_H
#define HLS_X_COMPLEX_H

#ifndef __SYNTHESIS__
#include <iostream>
#endif

#include "ap_fixed.h"

namespace std {
template <typename _Tp> class complex;
}

namespace hls {

/*
*******************************************************************************
* Complex class
*
* This is used over std::<complex> for the following reasons.
* 1. <complex> auto initializes all variables which forces an
*auto-initialization loop to be inserted for every declaration of an array of
*complex data
* 2. Using std::<complex> as base class leads to compilation errors.
*******************************************************************************
*/

template <typename T> class x_complex {
private:
  T re, im;

public:
  // Multiply return type
  typedef x_complex<T> MULT_RT;
  // default constructors
  inline x_complex() = default;
  inline ~x_complex() = default;
  inline x_complex(x_complex<T> &&) = default;
  inline x_complex(const x_complex<T> &z) = default;
  inline x_complex<T> &operator=(x_complex<T> &&) = default;
  inline x_complex<T> &operator=(const x_complex<T> &) = default;
  // other constructors
  inline x_complex(const T &r, const T &i) : re(r), im(i){};

  inline x_complex(const T &r) : re(r), im(0){};

  inline x_complex(const std::complex<T> &z) {
    x_complex<T> tmp(z.real(), z.imag());
    *this = tmp;
  }
  template <typename T2> inline x_complex(const x_complex<T2> &z) {
    x_complex<T2> tmp(z);
    x_complex<T> tmp2(tmp.real(), tmp.imag());
    *this = tmp2;
  }

  // Data access
  inline T real() const { return re; };
  inline T &real() { return re; };

  inline void real(const T &r) { re = r; };

  inline T imag() const { return (im); };
  inline T &imag() { return (im); };

  inline void imag(const T &i) { im = i; };

  // Operator =
  inline x_complex<T> &operator=(const T &rhs) {
    x_complex<T> tmp(rhs, 0);
    *this = tmp;
    return *this;
  };

  template <typename T2>
  inline x_complex<T> &operator=(const x_complex<T2> &rhs) {
    x_complex<T> tmp(rhs);
    *this = tmp;
    return *this;
  };

  // Operator *=, *
  inline x_complex<T> &operator*=(const T &rhs) {
    x_complex<T> tmp(*this);
    tmp.re *= rhs;
    tmp.im *= rhs;
    *this = tmp;
    return *this;
  };
  inline x_complex<T> &operator*=(const x_complex<T> &rhs) {
    x_complex<T> tmp_rhs(rhs);
    x_complex<T> tmp_this(*this);
    T tmp1 = tmp_this.re * tmp_rhs.real();
    T tmp2 = tmp_this.im * tmp_rhs.imag();
    T tmp3 = tmp_this.re * tmp_rhs.imag();
    T tmp4 = tmp_this.im * tmp_rhs.real();
    x_complex<T> tmp5(tmp1 - tmp2, tmp3 + tmp4);
    *this = tmp5;
    return *this;
  };
  template <typename T2>
  inline x_complex<T> &operator*=(const x_complex<T2> &rhs) {
    x_complex<T2> tmp(rhs);
    x_complex<T> tmp2(tmp.real(), tmp.imag());
    *this *= tmp2;
    return *this;
  };

  inline MULT_RT operator*(const T &rhs) {
    x_complex<T> tmp(*this);
    tmp *= rhs;
    return tmp;
  };
  inline MULT_RT operator*(const x_complex<T> &rhs) {
    x_complex<T> tmp(*this);
    tmp *= rhs;
    return tmp;
  }
  template <typename T2> inline MULT_RT operator*(const x_complex<T2> &rhs) {
    x_complex<T2> tmp(rhs);
    x_complex<T> tmp2(tmp.real(), tmp.imag());
    MULT_RT res = *this * tmp2;
    return res;
  };

  // Operator /=, /
  inline x_complex<T> &operator/=(const T &rhs) {
    x_complex<T> tmp(*this);
    tmp.re /= rhs;
    tmp.im /= rhs;
    *this = tmp;
    return *this;
  };
  inline x_complex<T> &operator/=(const x_complex<T> &rhs) {
    x_complex<T> tmp(rhs);
    x_complex<T> conj(tmp.real(), -tmp.imag());
    x_complex<T> a = (*this) * conj;
    x_complex<T> b = conj * rhs;
    x_complex<T> tmp2(a.real() / b.real(), a.imag() / b.real());
    *this = tmp2;
    return *this;
  };
  template <typename T2>
  inline x_complex<T> &operator/=(const x_complex<T2> &rhs) {
    x_complex<T2> tmp2(rhs);
    x_complex<T> tmp(tmp2.real(), tmp2.imag());
    *this /= tmp;
    return *this;
  };

  inline x_complex<T> operator/(const T &rhs) {
    x_complex<T> tmp(*this);
    tmp /= rhs;
    return tmp;
  };
  inline x_complex<T> operator/(const x_complex<T> &rhs) {
    x_complex<T> tmp(*this);
    tmp /= rhs;
    return tmp;
  };
  template <typename T2>
  inline x_complex<T> operator/(const x_complex<T2> &rhs) {
    x_complex<T> tmp(*this);
    tmp /= rhs;
    return tmp;
  };

  // Operator +=, +
  inline x_complex<T> &operator+=(const T &rhs) {
    re += rhs;
    return *this;
  };
  inline x_complex<T> operator+=(const x_complex<T> &rhs) {
    x_complex<T> tmp(rhs);
    x_complex<T> tmp2(*this);
    tmp2.re += tmp.real();
    tmp2.im += tmp.imag();
    *this = tmp2;
    return *this;
  };
  template <typename T2>
  inline x_complex<T> operator+=(const x_complex<T2> &rhs) {
    x_complex<T2> tmp(rhs);
    x_complex<T> tmp2(tmp.real(), tmp.imag());
    *this += tmp2;
    return *this;
  };

  inline x_complex<T> operator+(const T &rhs) {
    x_complex<T> tmp;
    tmp = *this;
    tmp += rhs;
    return tmp;
  };
  inline x_complex<T> operator+(const x_complex<T> &rhs) {
    x_complex<T> tmp;
    tmp = *this;
    tmp += rhs;
    return tmp;
  };
  template <typename T2>
  inline x_complex<T> operator+(const x_complex<T2> &rhs) {
    x_complex<T> tmp;
    tmp = *this;
    tmp += rhs;
    return tmp;
  };

  // Operator -=, -
  inline x_complex<T> &operator-=(const T &rhs) {
    re -= rhs;
    return *this;
  }
  inline x_complex<T> &operator-=(const x_complex<T> &rhs) {
    x_complex<T> tmp(rhs);
    x_complex<T> tmp2(*this);
    tmp2.re -= tmp.real();
    tmp2.im -= tmp.imag();
    *this = tmp2;
    return *this;
  };
  template <typename T2>
  inline x_complex<T> &operator-=(const x_complex<T2> &rhs) {
    x_complex<T2> tmp(rhs);
    x_complex<T> tmp2(*this);
    tmp2.re -= tmp.real();
    tmp2.im -= tmp.imag();
    *this = tmp2;
    return *this;
  };

  inline x_complex<T> operator-(const T &rhs) {
    x_complex<T> tmp;
    tmp = *this;
    tmp -= rhs;
    return tmp;
  };
  inline x_complex<T> operator-(const x_complex<T> &rhs) {
    x_complex<T> tmp;
    tmp = *this;
    tmp -= rhs;
    return tmp;
  };
  template <typename T2>
  inline x_complex<T> operator-(const x_complex<T2> &rhs) {
    x_complex<T> tmp;
    tmp = *this;
    tmp -= rhs;
    return tmp;
  };
  // neg
  inline x_complex<T> operator-() {
    x_complex<T> tmp(*this);
    tmp.real(-tmp.real());
    tmp.imag(-tmp.imag());
    return tmp;
  };

  // std::x_complex --> hls::x_complex
  x_complex<T> &operator=(const std::complex<T> &t) {
    std::complex<T> tmp(t);
    x_complex<T> tmp2(tmp.real(), tmp.imag());
    *this = tmp2;
    return *this;
  }
  // hls::x_complex --> std::x_complex
  operator std::complex<T>() const {
    x_complex<T> tmp(this);
    return std::complex<T>(tmp.re, tmp.im);
  }

  std::complex<T> to_std_complex() {
    x_complex<T> tmp(this);
    return std::complex<T>(tmp.re, tmp.im);
  }

#ifndef __SYNTHESIS__
  // Operator <<
  friend std::ostream &operator<<(std::ostream &out, const x_complex<T> &c) {
    bool neg_imag = c.imag() <= -0 ? true : false;
    T imag = neg_imag ? (T)-c.imag() : (T)c.imag();

    out << c.real() << (neg_imag ? " - j*" : " + j*") << imag;
    return out;
  }
#endif

}; // x_complex

// Non-member Operator ==, != for x_complex
template <typename T>
inline bool operator==(const T &lhs, const x_complex<T> &rhs) {
  x_complex<T> tmp(rhs);
  return (lhs == tmp.real()) && (0 == tmp.imag());
}
template <typename T>
inline bool operator==(const x_complex<T> &lhs, const T &rhs) {
  x_complex<T> tmp(lhs);
  return (tmp.real() == rhs) && (tmp.imag() == 0);
}
template <typename T>
inline bool operator==(const x_complex<T> &lhs, const x_complex<T> &rhs) {
  x_complex<T> tmpl(lhs);
  x_complex<T> tmpr(rhs);
  return (tmpl.real() == tmpr.real()) && (tmpl.imag() == tmpr.imag());
}

template <typename T>
inline bool operator!=(const T &lhs, const x_complex<T> &rhs) {
  x_complex<T> tmp(rhs);
  return (lhs != tmp.real()) || (0 != tmp.imag());
}
template <typename T>
inline bool operator!=(const x_complex<T> &lhs, const T &rhs) {
  x_complex<T> tmp(lhs);
  return (tmp.real() != rhs) || (tmp.imag() != 0);
}
template <typename T>
inline bool operator!=(const x_complex<T> &lhs, const x_complex<T> &rhs) {
  x_complex<T> tmpl(lhs);
  x_complex<T> tmpr(rhs);
  return (tmpl.real() != tmpr.real()) || (tmpl.imag() != tmpr.imag());
}

template <typename T> x_complex<T> x_neg(x_complex<T> &din) {
  x_complex<T> tmp(din);
  tmp.real(-tmp.real());
  tmp.imag(-tmp.imag());
  return (tmp);
}

template <typename T> x_complex<T> x_conj_sq(x_complex<T> &din) {
  x_complex<T> tmp(din);
  return ((tmp.real() * tmp.real()) + (tmp.imag() * tmp.imag()));
}

template <typename T> x_complex<T> x_conj(const x_complex<T> &din) {
  x_complex<T> tmp(din);
  tmp.imag(-tmp.imag());
  return tmp;
}

// This function is called when a non-const x_complex is conjugated.  It calls
// the version of x_conj() which takes a constant parameter
template <typename T> x_complex<T> x_conj(x_complex<T> &din) {
  const x_complex<T> tmp(din);
  return x_conj(tmp);
}

template <int W, int I> void set_to_one(x_complex<ap_fixed<W, I>> &a) {
  ap_int<W> tmp_sat = ((ap_int<W - I + 1>)1 << (W - I)) - 1;
  ap_fixed<W, I> tmp;
  tmp.range() = tmp_sat;
  a = tmp;
}

template <typename T> T x_real(const x_complex<T> &din) { return din.real(); }

template <typename T> T x_imag(const x_complex<T> &din) { return din.imag(); }

template <typename T1>
typename x_complex<T1>::MULT_RT operator*(const T1 &lhs, x_complex<T1> &rhs) {
  return rhs * lhs; // Flip direction and re-use complex class defined operator
}

} // end namespace hls

#endif

