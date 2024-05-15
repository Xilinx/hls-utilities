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


#ifndef X_HLS_HS_SIM_H
#define X_HLS_HS_SIM_H

/*
 * This file contains a C++ model of hls::ap_hs, hls::ap_vld, hls::ap_ack, hls::ap_none.
 * It defines their C simulation model.
 */
#ifndef __cplusplus

#error C++ is required to include this header file

#else

#include <assert.h>
#include "hls_stream.h"

namespace hls {

template<typename __STREAM_T__>
class ap_hs
{
  public:
    using value_type = __STREAM_T__;

  private:
  typedef stream_map<sizeof(__STREAM_T__)> map_t;

  protected:
#if defined(__VITIS_HLS__)
    __STREAM_T__ _data;
#else
    stream_entity<sizeof(__STREAM_T__)> _data;
#endif

  protected:
  public:
    /// Constructors
    // Keep consistent with the synthesis model's constructors
    ap_hs() {
      std::stringstream ss;
#ifndef _MSC_VER
      char* _demangle_name = abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
      if (_demangle_name) {
        ss << _demangle_name;
        free(_demangle_name);
      }
      else {
        ss << "hls_ap_hs";
      }
#else
      ss << typeid(*this).name();
#endif

#ifdef HLS_STREAM_THREAD_UNSAFE
      static unsigned counter = 0;
#else
      static std::atomic<unsigned> counter(0);
#endif

#if defined(__VITIS_HLS__)
      map_t::insert(&_data);
#endif
      ss << counter++;
      get_entity().set_name(ss.str().c_str());
    }

    ap_hs(const char *name) {
    // default constructor,
    // capacity set to predefined maximum
#if defined(__VITIS_HLS__)
      map_t::insert(&_data);
#endif
      get_entity().set_name(name);
    }

  /// Make copy constructor and assignment operator private
  /// They should not be called.
  private:
    ap_hs(const ap_hs< __STREAM_T__ >& chn):
        _data(chn._data) {
    }

    ap_hs& operator = (const ap_hs< __STREAM_T__ >& chn) {
        return *this;
    }

    stream_entity<sizeof(__STREAM_T__)> &get_entity() {
#if defined(__VITIS_HLS__)
      return map_t::get_entity(&_data);
#else
      return _data;
#endif
    }

  public:
    size_t size() {
      return get_entity().size();
    }

    /// Destructor
    /// Check status of the queue
    ~ap_hs() {
      if (valid())
      {
          std::cout << "WARNING [HLS SIM]: hls::ap_hs '" 
                    << get_entity().name
                    << "' contains leftover data,"
                    << " which may result in RTL simulation hanging."
                    << std::endl;
      }
    }

#if defined(__VITIS_HLS__)
    bool exist() {
      return map_t::count(&_data);
    }
#endif

    /// Status of the queue
    bool valid() {
      return size() != 0;
    }    

    bool ready() const { return true; }

    /// Blocking read
    void read(__STREAM_T__& head) {
        head = read();
    }

    /// Blocking read
    __STREAM_T__ read() {
      __STREAM_T__ elem;
      auto &entity = get_entity();
      if (!entity.read(&elem)) 
        elem = __STREAM_T__();
      return elem;
    }

    /// Blocking write
    void write(const __STREAM_T__& tail) { 
      get_entity().write(&tail);
    }

    /// Nonblocking read
    bool read_nb(__STREAM_T__& head) {
      __STREAM_T__ elem;
      bool vld = valid();
      if (vld)
          read(elem); 
      return vld;
    }

    /// Nonblocking write
    bool write_nb(const __STREAM_T__& tail) {
        bool rdy = ready();
        if (rdy)
            write(tail);
            return rdy;
    }

    /// Set name for c-sim debugging.
    void set_name(const char *name) { 
      get_entity().set_name(name);
    }

    void set_delegate(stream_delegate<sizeof(__STREAM_T__)> *d) {
      get_entity().d = d;
    }
};

} // namespace hls

#endif // __cplusplus
#endif  // X_HLS_HS_H


