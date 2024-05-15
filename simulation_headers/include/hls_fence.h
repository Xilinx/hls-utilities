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
static void fence(const internal::fence_group &A, const internal::fence_group &B) {
  #pragma HLS inline
  __fpga_fence_with_group(A.p, B.p);
}
 
// bi-direction fence
template <class... Args>
void fence(Args&&... args) {
  #pragma HLS inline
  __fpga_fence(internal::auto_type_convert(args)...);
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

static void fence(const internal::fence_group &A, const internal::fence_group &B) {
  std::atomic_thread_fence(std::memory_order_seq_cst);
}

template <class... Args>
void fence(Args&&... args) {
  std::atomic_thread_fence(std::memory_order_seq_cst);
}
}
#endif
#endif
#endif
