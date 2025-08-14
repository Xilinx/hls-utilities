// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef X_HLS_NP_CHANNEL_SIM_H
#define X_HLS_NP_CHANNEL_SIM_H

#include "hls_stream.h"
#include <string.h>
namespace hls {
template <typename T, unsigned N_OUT_PORTS, unsigned N_IN_PORTS>
class load_balancing_np : public stream_delegate<sizeof(T)> {
private:
  std::string name;
  std::deque<std::array<char, sizeof(T)> > _data;
#ifndef HLS_STREAM_THREAD_UNSAFE
  std::mutex _mutex;
  std::condition_variable _condition_var;
  bool invalid;
#endif
protected:
#ifdef HLS_STREAM_THREAD_UNSAFE
  load_balancing_np() {}
  load_balancing_np(const char *n) : name(n) {}
#else
  load_balancing_np(): invalid(false) {}
  load_balancing_np(const char *n) : name(n), invalid(false) {}
  ~load_balancing_np() {
    std::unique_lock<std::mutex> ul(_mutex);
    invalid = true;
    _condition_var.notify_all();
  }
#endif

public:
  virtual size_t size() {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::lock_guard<std::mutex> lg(_mutex);
#endif
    return _data.size();
  }

  virtual bool read(void *elem) {
#ifdef ALLOW_EMPTY_HLS_STREAM_READS
    if (_data.empty()) {
      std::cout << "WARNING [HLS SIM]: n-port channel '"
                << name
                << "' is read while empty,"
                << " which may result in RTL simulation hanging."
                << std::endl;
      return false;
    }
#else
    std::unique_lock<std::mutex> ul(_mutex);
    stream_globals<false>::incr_blocked_counter();
    while (_data.empty()) {
#ifndef HLS_STREAM_THREAD_UNSAFE
      while (invalid) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      _condition_var.wait(ul);
#endif
    }
    stream_globals<false>::decr_blocked_counter();
#endif
    
    memcpy(elem, _data.front().data(), sizeof(T));
    _data.pop_front();
    return true;
  }

  virtual bool read_nb(void *elem) {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::lock_guard<std::mutex> lg(_mutex);
#endif
    bool is_empty = _data.empty();
    if (!is_empty) {
      memcpy(elem, _data.front().data(), sizeof(T));
      _data.pop_front();
    }
    return !is_empty;
  }

  virtual void write(const void *elem) {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::unique_lock<std::mutex> ul(_mutex);
#endif
    std::array<char, sizeof(T)> elem_data;
    memcpy(elem_data.data(), elem, sizeof(T));
    _data.push_back(elem_data);
    if (stream_globals<false>::get_max_size() < _data.size())
        stream_globals<false>::get_max_size() = _data.size();
#ifndef HLS_STREAM_THREAD_UNSAFE
    _condition_var.notify_one();
#endif
  }
};

namespace split {
template <typename T, unsigned N_OUT_PORTS, unsigned DEPTH = 2>
class load_balance : public load_balancing_np<T, N_OUT_PORTS, 1> { 
public:
  stream<T> in;
  stream<T> out[N_OUT_PORTS];
  load_balance() {
    in.set_delegate(this);
    for (int i = 0; i < N_OUT_PORTS; i++)
      out[i].set_delegate(this);
  }

  load_balance(const char *name) : load_balancing_np<T, N_OUT_PORTS, 1>(name) {
    in.set_delegate(this);
    for (int i = 0; i < N_OUT_PORTS; i++)
      out[i].set_delegate(this);
  }
};

template <typename T, unsigned N_OUT_PORTS, unsigned ONEPORT_DEPTH = 2, unsigned NPORT_DEPTH = 0>
class round_robin : public stream_delegate<sizeof(T)> {
private:
#ifndef HLS_STREAM_THREAD_UNSAFE
  std::mutex _mutex;
#endif
  std::string name;
  int pos;
public:
  stream<T> in;
  stream<T> out[N_OUT_PORTS];
  round_robin() : pos(0) {
    in.set_delegate(this);
  }

  round_robin(const char *n) : name(n), pos(0) {
    in.set_delegate(this);
  }

  virtual size_t size() {
    return 0;
  }

  virtual bool read(void *elem) {
    std::cout << "WARNING [HLS SIM]: the 'in' port of n-port channel '"
              << name
              << "' cannot be read."
              << std::endl;
    return false;
  }

  virtual bool read_nb(void *elem) {
    return read(elem);
  }

  virtual void write(const void *elem) {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::lock_guard<std::mutex> lg(_mutex);
#endif
    out[pos].write(*(T *)elem);
    pos = (pos + 1) % N_OUT_PORTS;
  }
};
} // end of namespace split

namespace merge {

template <typename T, unsigned N_IN_PORTS, unsigned DEPTH = 2>
class load_balance : public load_balancing_np<T, 1, N_IN_PORTS> { 
public:
  stream<T> in[N_IN_PORTS];
  stream<T> out;
  load_balance() {
    out.set_delegate(this);
    for (int i = 0; i < N_IN_PORTS; i++)
      in[i].set_delegate(this);
  }

  load_balance(const char *name) : load_balancing_np<T, 1, N_IN_PORTS>(name) {
    out.set_delegate(this);
    for (int i = 0; i < N_IN_PORTS; i++)
      in[i].set_delegate(this);
  }
};

template <typename T, unsigned N_IN_PORTS, unsigned ONEPORT_DEPTH = 2, unsigned NPORT_DEPTH = 0>
class round_robin : public stream_delegate<sizeof(T)> { 
private:
#ifndef HLS_STREAM_THREAD_UNSAFE
  std::mutex _mutex;
#endif
  std::string name;
  int pos;
public:
  stream<T> in[N_IN_PORTS];
  stream<T> out;
  round_robin() : pos(0) {
    out.set_delegate(this);
  }

  round_robin(const char *n) : name(n), pos(0) {
    out.set_delegate(this);
  }

  virtual size_t size() {
    return in[pos].size();
  }

  virtual bool read(void *elem) {
#ifdef HLS_STREAM_THREAD_UNSAFE
    if (in[pos].empty())
      return false;
#else
    std::lock_guard<std::mutex> lg(_mutex);
#endif
    in[pos].read(*(T *)elem);
    pos = (pos + 1) % N_IN_PORTS;
    return true;
  }

  virtual bool read_nb(void *elem) {
#ifndef HLS_STREAM_THREAD_UNSAFE
    std::lock_guard<std::mutex> lg(_mutex);
#endif
    if (in[pos].read_nb(*(T *)elem)) {
      pos = (pos + 1) % N_IN_PORTS;
      return true;
    }
    return false; 
  }  

  virtual void write(const void *elem) {
    std::cout << "WARNING [HLS SIM]: the 'out' port of n-port channel '"
              << name
              << "' cannot be written."
              << std::endl;
  }
};
} // end of namespace merge
} // end of namespace hls
#endif
