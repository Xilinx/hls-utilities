// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef X_HLS_STREAMOFBLOCKS_H
#define X_HLS_STREAMOFBLOCKS_H

#ifndef __cplusplus
#error C++ is required to include this header file
#else

#include <queue>
#include <iostream>
#include <typeinfo>
#include <string>
#include <sstream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "hls_stream.h"

#ifndef AP_INLINE
#define AP_INLINE inline 
#endif
#define UNUSED_ARG 

#ifndef __has_attribute
#define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

#if __has_attribute(no_ctor)
#define NO_CTOR __attribute__((no_ctor))
#else
#define NO_CTOR
#endif

namespace hls {
template <typename __STREAM_T__>
class stream_buf {
  std::string name;
  std::deque<__STREAM_T__*> data;
  int readLocks;
  int writeLocks;
 
 public:
  AP_INLINE stream_buf(int depth, const char *n)
    : name(n ? n : "stream_of_blocks"), readLocks(0), 
      writeLocks(0), invalid(false){}

  ~stream_buf() {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::unique_lock<std::mutex> ul(mutex);
    invalid = true;
    condition_var.notify_all();
#endif
  }

  void write_check() {
    if (writeLocks <= 0) {
        std::cerr << "ERROR: writing " << name << " before acquiring." << std::endl;
        abort();
    }
  }
  void read_check() {
    if (readLocks <= 0) {
        std::cerr << "ERROR: reading " << name << " before acquiring." << std::endl;
        abort();
    }
  }

  AP_INLINE __STREAM_T__& read_acquire() {
    // needed to start the deadlock detector and size reporter
    stream_globals<false>::start_threads();

    if (readLocks > 0) {
        std::cerr << "ERROR: acquiring " << name << " for reading more than once before releasing. Use braces to limit the lifetime of the lock object." << std::endl;
        abort();
    }
    readLocks++;
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::unique_lock<std::mutex> ul(mutex);
#endif
    if (!data.size()) { 
#ifdef ALLOW_EMPTY_HLS_STREAM_READS
      std::cout << "WARNING [HLS SIM]: hls::stream_of_blocks '"
                << name
                << "' is read while empty,"
                << " which may result in RTL simulation hanging."
                << std::endl;
        return *new __STREAM_T__[1];
#else
        stream_globals<false>::incr_blocked_counter();
        while (!data.size()) {
#ifndef HLS_STREAM_THREAD_UNSAFE
          while (invalid) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
           }
          condition_var.wait(ul);
#endif
        }
        stream_globals<false>::decr_blocked_counter();
#endif
    }
    return *data.front();
  }
 
  AP_INLINE void read_release() {
    readLocks--;
    if (readLocks != 0) {
        std::cerr << "INTERNAL ERROR: releasing " << name << " for reading too many times." << std::endl;
        abort();
    }
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::unique_lock<std::mutex> ul(mutex);
#endif
    data.pop_front();
  }
 
  AP_INLINE __STREAM_T__& write_acquire() {
    // needed to start the deadlock detector and size reporter
    stream_globals<false>::start_threads();

    if (writeLocks > 0) {
        std::cerr << "ERROR: acquiring " << name << " for writing more than once before releasing. Use braces to limit the lifetime of the lock object." << std::endl;
        abort();
    }
    writeLocks++;

#ifndef HLS_STREAM_THREAD_UNSAFE
    std::unique_lock<std::mutex> ul(mutex);
#endif
    data.push_back(new __STREAM_T__[1]);
#ifndef HLS_STREAM_THREAD_UNSAFE
    condition_var.notify_one();
#endif
    return *data.back();
  }

  AP_INLINE void write_release() {
    writeLocks--;
    if (writeLocks != 0) {
        std::cerr << "INTERNAL ERROR: releasing " << name << " for writing too many times." << std::endl;
        abort();
    }
  }
 
  AP_INLINE bool empty() {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::unique_lock<std::mutex> ul(mutex);
#endif
    return !data.size();
  }

  AP_INLINE bool full() {
    return false;
  }

#ifdef EXPLICIT_ACQUIRE_RELEASE
  template <typename>
  friend class read_buf;
  template <typename>
  friend class write_buf;
#endif

  template <typename>
  friend class read_lock;
  template <typename>
  friend class write_lock;
#ifndef HLS_STREAM_THREAD_UNSAFE
  std::mutex mutex;
  std::condition_variable condition_var;
  bool invalid;
#endif
};
 
template<typename __STREAM_T__, int DEPTH=2>
class stream_of_blocks;

#ifdef EXPLICIT_ACQUIRE_RELEASE
template <typename __STREAM_T__>
class read_buf {
  stream_of_blocks<__STREAM_T__>& res;
  __STREAM_T__* buf;
 
 public:
  AP_INLINE read_buf(stream_of_blocks<__STREAM_T__>& s) : res(s) {
  }

  AP_INLINE void acquire() {
    buf = &res.read_acquire();
  }

  AP_INLINE void release() {
    res.read_release();
  }

  AP_INLINE ~read_buf() { }

  AP_INLINE operator __STREAM_T__&() { 
    res.buf.read_check();
    return *buf;
  }

  AP_INLINE __STREAM_T__& operator=(const __STREAM_T__& val) { 
    res.buf.read_check();
    buf = &val; 
    return *buf; 
  }
};
 
template <typename __STREAM_T__>
class write_buf {
  stream_of_blocks<__STREAM_T__>& res;
  __STREAM_T__* buf;
 
 public:
  AP_INLINE write_buf(stream_of_blocks<__STREAM_T__>& s) : res(s) {
  }

  AP_INLINE void acquire() {
    buf = &res.write_acquire();
  }

  AP_INLINE void release() {
    res.write_release();
  }

  AP_INLINE ~write_buf() { }

  AP_INLINE operator __STREAM_T__&() { 
    res.buf.write_check();
    return *buf; 
  }

  AP_INLINE __STREAM_T__& operator=(const __STREAM_T__& val) {
    res.buf.write_check();
    buf = &val; 
    return *buf; 
  }
};
#endif

template <typename __STREAM_T__>
class read_lock {
  stream_of_blocks<__STREAM_T__>& res;
  __STREAM_T__& buf;
 
 public:
  AP_INLINE read_lock(stream_of_blocks<__STREAM_T__>& s) : res(s), buf(res.read_acquire()) { }

  AP_INLINE ~read_lock() { res.read_release(); }

  AP_INLINE operator __STREAM_T__&() { return buf; }

  AP_INLINE __STREAM_T__& operator=(const __STREAM_T__& val) { return buf = val; }
};
 
template <typename __STREAM_T__>
class write_lock {
  stream_of_blocks<__STREAM_T__>& res;
  __STREAM_T__& buf;
 
 public:
  AP_INLINE write_lock(stream_of_blocks<__STREAM_T__>& s) : res(s), buf(res.write_acquire()) { }

  AP_INLINE ~write_lock() { res.write_release(); }

  AP_INLINE operator __STREAM_T__&() { return buf; }

  AP_INLINE __STREAM_T__& operator=(const __STREAM_T__& val) { return buf = val; }
};
 
template <typename __STREAM_T__>
class stream_of_blocks<__STREAM_T__, 2> {
#ifdef EXPLICIT_ACQUIRE_RELEASE
  friend class read_buf<__STREAM_T__>; 
  friend class write_buf<__STREAM_T__>; 
#endif
  friend class read_lock<__STREAM_T__>; 
  friend class write_lock<__STREAM_T__>; 

  stream_buf<__STREAM_T__> buf;
 
 public:
  AP_INLINE stream_of_blocks(int depth=2, UNUSED_ARG char *name=0): buf(depth, name) { }

  AP_INLINE bool full() { return buf.full(); }

  AP_INLINE bool empty() { return buf.empty(); }

 private:
  AP_INLINE __STREAM_T__& read_acquire() { return buf.read_acquire(); }

  AP_INLINE void read_release() { buf.read_release(); }
 
  AP_INLINE __STREAM_T__& write_acquire() { return buf.write_acquire(); }

  AP_INLINE void write_release() { buf.write_release(); }

  //__STREAM_T__& read(); TBD
  //void write(const __STREAM_T__&); TBD
};

template <typename __STREAM_T__, int DEPTH>
class stream_of_blocks: public stream_of_blocks<__STREAM_T__, 2> {
#ifdef EXPLICIT_ACQUIRE_RELEASE
  friend class read_buf<__STREAM_T__>; 
  friend class write_buf<__STREAM_T__>; 
#endif
  friend class read_lock<__STREAM_T__>; 
  friend class write_lock<__STREAM_T__>; 

 public:
  AP_INLINE stream_of_blocks(): stream_of_blocks<__STREAM_T__, 2>(DEPTH) {}
};

} // end of namespace hls
#endif
#endif
