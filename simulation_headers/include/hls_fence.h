// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef X_HLS_FENCE_H
#define X_HLS_FENCE_H
#ifndef __cplusplus

#error C++ is required to include this header file

#else
#include "hls_streamofblocks.h"
#ifdef __SYNTHESIS__
namespace hls {
namespace internal {
  template<typename __STREAM_T__>
  __STREAM_T__& auto_type_convert(read_lock<__STREAM_T__> &t) {
  #pragma HLS inline
    return (__STREAM_T__&)(t);
  };

  template<typename __STREAM_T__>
  __STREAM_T__& auto_type_convert(write_lock<__STREAM_T__> &t) {
  #pragma HLS inline
    return (__STREAM_T__&)(t);
  };

  template<typename T>
  T& auto_type_convert(T& t) {
  #pragma HLS inline
    return t;
  };

  struct fence_group {
    template<class... Args>
    fence_group(Args&&... args) {
      #pragma HLS inline
      p = __fpga_fence_group(auto_type_convert(args)...);
    }

    int p;
  };
} // end of namespace internal

// one-direction fence
template <unsigned DELAY = 1>
static void fence(const internal::fence_group &A, const internal::fence_group &B) {
  #pragma HLS inline
  __fpga_fence_with_group(A.p, B.p, DELAY);
}
 
// bi-direction fence
template <unsigned DELAY = 1, class... Args>
void fence(Args&&... args) {
  #pragma HLS inline
  __fpga_fence(DELAY, internal::auto_type_convert(args)...);
}

}
#else
#include <atomic>
namespace hls {
namespace internal {
  struct fence_group {
    template<class... Args>
    fence_group(Args&&... args) {} // dummy struct
    int p;
  };
} // end of namespace internal

template <unsigned DELAY = 1>
static void fence(const internal::fence_group &A, const internal::fence_group &B) {
  std::atomic_thread_fence(std::memory_order_seq_cst);
}

template <unsigned DELAY = 1, class... Args>
void fence(Args&&... args) {
  std::atomic_thread_fence(std::memory_order_seq_cst);
}
}
#endif
#endif
#endif
