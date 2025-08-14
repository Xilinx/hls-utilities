// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef __HLS_VECTOR_H__
#define __HLS_VECTOR_H__

#include <array>
#include <cassert>
#include <cstddef>
#include <initializer_list>

namespace hls {

#ifdef __SYNTHESIS__
#define SYN_PRAGMA(PRAG) _Pragma(#PRAG)
#ifndef AP_NODEBUG
#define AP_NODEBUG __attribute__((nodebug))
#endif // AP_NODEBUG
#else
#define SYN_PRAGMA(PRAG)
#ifndef AP_NODEBUG
#define AP_NODEBUG
#endif // AP_NODEBUG
#endif // __SYNTHESIS__

#ifdef __SYNTHESIS__
#ifndef AP_INLINE
#define AP_INLINE inline __attribute__((always_inline))
#endif
#else
#ifndef AP_INLINE
#define AP_INLINE inline
#endif
#endif // __SYNTHESIS__

namespace details {

/// Returns the greatest power of two that divides N
constexpr size_t gp2(size_t N) {
  if (N == 0)
    return 0;
  if (N % 2 != 0)
    return 1;
  return 2 * gp2(N / 2);
}

} // namespace details

/// SIMD Vector of `N` elements of type `T`
template <typename T, size_t N>
class alignas(details::gp2(sizeof(T) * N)) vector {
  static_assert(N > 0, "vector must have at least one element");

  using data_t = std::array<T, N>;
  data_t data;

public:
#define USING_TYPE(NAME) using NAME = typename data_t::NAME
  /// Member types
  USING_TYPE(value_type);
  USING_TYPE(size_type);
  USING_TYPE(difference_type);
  USING_TYPE(reference);
  USING_TYPE(const_reference);
  USING_TYPE(pointer);
  USING_TYPE(const_pointer);
  USING_TYPE(iterator);
  USING_TYPE(const_iterator);
  USING_TYPE(reverse_iterator);
  USING_TYPE(const_reverse_iterator);
#undef USING_TYPE

#define MUTABLE_MEMBER(RET, NAME)                                              \
  RET NAME() {                                                                 \
    pragma();                                                                  \
    return data.NAME();                                                        \
  }

#define CONST_MEMBER(RET, NAME)                                                \
  RET NAME() const {                                                           \
    pragma();                                                                  \
    return data.NAME();                                                        \
  }

  MUTABLE_MEMBER(iterator, begin)
  MUTABLE_MEMBER(iterator, end)
  MUTABLE_MEMBER(reverse_iterator, rbegin)
  MUTABLE_MEMBER(reverse_iterator, rend)
  CONST_MEMBER(const_iterator, begin)
  CONST_MEMBER(const_iterator, end)
  CONST_MEMBER(const_iterator, cbegin)
  CONST_MEMBER(const_iterator, cend)
  CONST_MEMBER(const_reverse_iterator, rbegin)
  CONST_MEMBER(const_reverse_iterator, rend)
  CONST_MEMBER(const_reverse_iterator, crbegin)
  CONST_MEMBER(const_reverse_iterator, crend)

  CONST_MEMBER(bool, empty)
  CONST_MEMBER(size_type, size)
  CONST_MEMBER(size_type, max_size)

#undef MUTABLE_MEMBER
#undef CONST_MEMBER

protected:
  /// Pragma setter (hack until we support pragma on types)
  /// Note: must be used on all functions if possible
  AP_INLINE AP_NODEBUG void pragma() const {
    SYN_PRAGMA(HLS AGGREGATE variable=this)
  }

public:
  /// Default constructor (trivial)
  vector() = default;
  /// Copy-constructor (trivial)
  vector(const vector &other) = default;
  /// Move-constructor (trivial)
  vector(vector &&other) = default;
  /// Copy-assignment operator (trivial)
  vector &operator=(const vector &other) = default;
  /// Move-assignment operator (trivial)
  vector &operator=(vector &&other) = default;
  /// Destructor (trivial)
  ~vector() = default;

  /// Note: all the above special member functions must be trivial,
  ///       as we want this class to be usable in union (POD requirement).

  /// Construct from T (scalar)
  AP_INLINE vector(const T &val) {
    pragma();
    for (size_t i = 0; i < N; ++i) {
      SYN_PRAGMA(HLS UNROLL)
      data[i] = val;
    }
  }

  /// Construct from std::array<T, N>
  AP_INLINE AP_NODEBUG vector(const std::array<T, N> &data) : data{data} {
    pragma();
  }

  /// Construct from std::initializer_list<T>
  AP_INLINE vector(std::initializer_list<T> l) {
    pragma();
    assert(l.size() == N &&
           "Initializer list must be the same size as the vector");
    for (size_t i = 0; i < N; ++i) {
      SYN_PRAGMA(HLS UNROLL)
      data[i] = l.begin()[i];
    }
  }

  /// Array-like operator[]
  AP_INLINE AP_NODEBUG T &operator[](size_t idx) {
    pragma();
    return data[idx];
  }
  AP_INLINE AP_NODEBUG const T &operator[](size_t idx) const {
    pragma();
    return data[idx];
  }

#define INPLACE_PREUNOP(OP)                                                    \
  AP_INLINE vector &operator OP() {                                       \
    pragma();                                                                  \
    for (size_t i = 0; i < N; ++i) {                                           \
      SYN_PRAGMA(HLS UNROLL)                                                   \
      OP data[i];                                                              \
    }                                                                          \
    return *this;                                                              \
  }

  INPLACE_PREUNOP(++)
  INPLACE_PREUNOP(--)

#undef INPLACE_PREUNOP

#define INPLACE_POSTUNOP(OP)                                                   \
 AP_INLINE AP_NODEBUG vector operator OP(int) {                                      \
    pragma();                                                                  \
    vector orig = *this;                                                       \
    OP *this;                                                                  \
    return orig;                                                               \
  }

  INPLACE_POSTUNOP(++)
  INPLACE_POSTUNOP(--)

#undef INPLACE_POSTUNOP

#define INPLACE_BINOP(OP)                                                      \
 AP_INLINE vector &operator OP(const vector &rhs) {                       \
    pragma();                                                                  \
    rhs.pragma();                                                              \
    for (size_t i = 0; i < N; ++i) {                                           \
      SYN_PRAGMA(HLS UNROLL)                                                   \
      data[i] OP rhs[i];                                                       \
    }                                                                          \
    return *this;                                                              \
  }

  INPLACE_BINOP(+=)
  INPLACE_BINOP(-=)
  INPLACE_BINOP(*=)
  INPLACE_BINOP(/=)
  INPLACE_BINOP(%=)
  INPLACE_BINOP(&=)
  INPLACE_BINOP(|=)
  INPLACE_BINOP(^=)
  INPLACE_BINOP(<<=)
  INPLACE_BINOP(>>=)

#undef INPLACE_BINOP

#define REDUCE_OP(NAME, OP)                                                    \
  AP_INLINE T reduce_##NAME() const {                                             \
    pragma();                                                                  \
    T res = data[0];                                                           \
    for (size_t i = 1; i < N; ++i) {                                           \
      SYN_PRAGMA(HLS UNROLL)                                                   \
      res OP data[i];                                                          \
    }                                                                          \
    return res;                                                                \
  }

  REDUCE_OP(add,  +=)
  REDUCE_OP(mult, *=)
  REDUCE_OP(and,  &=)
  REDUCE_OP(or,   |=)
  REDUCE_OP(xor,  ^=)

#undef REDUCE_OP

#define LEXICO_OP(OP) \
  AP_INLINE friend bool operator OP(const vector &lhs, const vector &rhs) {       \
    lhs.pragma();                                                              \
    rhs.pragma();                                                              \
    for (size_t i = 0; i < N; ++i) {                                           \
      SYN_PRAGMA(HLS UNROLL)                                                   \
      if (lhs[i] == rhs[i])                                                    \
        continue;                                                              \
      return lhs[i] OP rhs[i];                                                 \
    }                                                                          \
    return T{} OP T{};                                                         \
  }

#define COMPARE_OP(OP)                                                         \
  AP_INLINE friend vector<bool, N> operator OP(const vector &lhs,                 \
                                            const vector &rhs) {               \
    lhs.pragma();                                                              \
    rhs.pragma();                                                              \
    vector<bool, N> res;                                                       \
    for (size_t i = 0; i < N; ++i) {                                           \
      SYN_PRAGMA(HLS UNROLL)                                                   \
      res[i] = lhs[i] OP rhs[i];                                               \
    }                                                                          \
    return res;                                                                \
  }

  LEXICO_OP(<)
  LEXICO_OP(<=)
  LEXICO_OP(==)
  LEXICO_OP(!=)
  LEXICO_OP(>=)
  LEXICO_OP(>)

#undef LEXICO_OP
#undef COMPARE_OP

#define BINARY_OP(OP, INPLACE_OP)                                              \
  AP_INLINE friend vector operator OP(vector lhs, const vector &rhs) {            \
    lhs.pragma();                                                              \
    rhs.pragma();                                                              \
    return lhs INPLACE_OP rhs;                                                 \
  }

  BINARY_OP(+, +=)
  BINARY_OP(-, -=)
  BINARY_OP(*, *=)
  BINARY_OP(/, /=)
  BINARY_OP(%, %=)
  BINARY_OP(&, &=)
  BINARY_OP(|, |=)
  BINARY_OP(^, ^=)
  BINARY_OP(<<, <<=)
  BINARY_OP(>>, >>=)

#undef BINARY_OP
};

} // namespace hls

#endif
