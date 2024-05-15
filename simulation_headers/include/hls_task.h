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


#ifndef X_HLS_TASK_H
#define X_HLS_TASK_H
#include <functional>

#define HLS_TASK hls_thread_local hls::task
#define HLS_TASK_STREAM hls_thread_local hls::stream
#define HLS_TASK_STREAM_OF_BLOCKS hls_thread_local hls::stream_of_blocks
#define HLS_TASK_MERGE hls_thread_local hls::merge
#define HLS_TASK_SPLIT hls_thread_local hls::split

#ifdef __SYNTHESIS__
#include "hls_stream.h"
#include "hls_streamofblocks.h"

#else
// hls_task requires multi-thread version of hls::stream
#if defined(X_HLS_STREAM_SIM_H) && defined(HLS_STREAM_THREAD_UNSAFE)
#error "Don't use thread unsafe hls_stream.h with hls_task.h"
#endif
#if defined(X_HLS_STREAMOFBLOCKS_H) && defined(HLS_STREAM_THREAD_UNSAFE)
#error "Don't use thread unsafe hls_streamofblocks.h with hls_task.h"
#endif
#if defined(X_HLS_NP_CHANNEL_H) && defined(HLS_STREAM_THREAD_UNSAFE)
#error "Don't use thread unsafe hls_np_channel.h with hls_task.h"
#endif
#if defined(HLS_STREAM_THREAD_UNSAFE)
#error "Don't use thread unsafe streams.h with hls_task.h"
#endif
#if !defined(__HLS_COSIM__) && defined(__VITIS_HLS__)
#error "hls::tasks are not supported in bcsim"
#endif

#include "hls_stream.h"
#include "hls_streamofblocks.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#endif // __SYNTHESIS__

namespace hls {

template<typename T>
struct is_hls_streaming_type : std::false_type {};

template<typename T, int DEPTH>
struct is_hls_streaming_type<hls::stream<T, DEPTH>&>: std::true_type {};

template<typename T, int DEPTH, int SIZE>
struct is_hls_streaming_type<hls::stream<T, DEPTH>(&)[SIZE]>: std::true_type {};

template<typename T, int DEPTH>
struct is_hls_streaming_type<hls::stream_of_blocks<T, DEPTH>&>: std::true_type {};

template<typename T, int DEPTH, int SIZE>
struct is_hls_streaming_type<hls::stream_of_blocks<T, DEPTH>(&)[SIZE]>: std::true_type {};

template < typename... List >
struct good_types_for_hls_task : std::true_type {};

template < typename Head, typename... Rest >
struct good_types_for_hls_task<Head, Rest...>
: std::conditional<is_hls_streaming_type<Head>::value && good_types_for_hls_task<Rest...>::value,
    std::true_type,
    std::false_type
>::type {};

template <>
struct good_types_for_hls_task<> : std::true_type {};

#ifdef __SYNTHESIS__
class task {
  int tag;
public:
  task() {
  }
  template <class T, class... Args>
  void operator()(T &&fn, Args&&... args) {
  #pragma HLS inline
     __attribute__((xlx_infinite_task(&tag))) fn(args...);
  }
  template <class T, class... Args>
  task(T &&fn, Args&&... args) {
  #pragma HLS inline
     __attribute__((xlx_infinite_task(&tag))) fn(args...);
  }
};

#define hls_thread_local

#else 
class task {
public:
  task() {
    stream_globals::incr_task_counter();
  }
  ~task() {
  }
  template <class T, class... Args>
  void operator()(T fn, Args&&... args) {
    std::thread tmp(t_wrapper<T, decltype(auto_ref(args))...>, fn, auto_ref(args)...);
    tmp.detach();
  }
  template <class T, class... Args>
  task(T fn, Args&&... args) {
    stream_globals::incr_task_counter();
    std::thread tmp(t_wrapper<T, decltype(auto_ref(args))...>, fn, auto_ref(args)...);
    tmp.detach();
  }
private:
  template <class T, class...Args>
  static void t_wrapper(T fn, Args... args) {
    while(1) {
      fn(args...);
    }
  }

  template<typename T, int DEPTH>
  std::reference_wrapper<hls::stream<T, DEPTH>> auto_ref(hls::stream<T, DEPTH> &elem) {
    return std::ref(elem);
  }

  template<typename T, int DEPTH>
  std::reference_wrapper<hls::stream_of_blocks<T, DEPTH>> auto_ref(hls::stream_of_blocks<T, DEPTH> &elem) {
    return std::ref(elem);
  }

  template<typename T>
  T auto_ref(T &elem) {
    return elem;
  }

};

#define hls_thread_local thread_local
#endif // __SYNTHESIS__
} 
#endif
