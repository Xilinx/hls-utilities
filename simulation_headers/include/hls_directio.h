// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef X_HLS_DIRECTIO_SIM_H
#define X_HLS_DIRECTIO_SIM_H

/*
 * This file contains a C++ model of hls::ap_hs, hls::ap_vld, hls::ap_ack, hls::ap_none.
 * It defines their C simulation model.
 */
#ifndef __cplusplus

#error C++ is required to include this header file

#else

#include <assert.h>
#include "hls_stream.h"
#include <type_traits> // Include this header for std::enable_if

namespace hls {

enum class mode : int { hs = 0, vld = 1, ack = 2, none = 3 };

template<typename __STREAM_T__, int __MODE__ = static_cast<int>(mode::hs)>
class directio
{
  public:
    using value_type = __STREAM_T__;

  private:
  typedef stream_map<sizeof(__STREAM_T__), true> map_t;

  protected:
#if defined(__VITIS_HLS__)
    __STREAM_T__ _data;
#else
    stream_entity<sizeof(__STREAM_T__), true> _data;
#endif

  protected:
  public:
    /// Constructors
    // Keep consistent with the synthesis model's constructors
    directio() {
      std::stringstream ss;
#ifndef _MSC_VER
      char* _demangle_name = abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
      if (_demangle_name) {
        ss << _demangle_name;
        free(_demangle_name);
      }
      else {
        ss << "hls_directio";
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

    directio(const char *name) {
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
    directio(const directio< __STREAM_T__, __MODE__ >& chn):
        _data(chn._data) {
    }

    directio& operator = (const directio< __STREAM_T__, __MODE__ >& chn) {
        return *this;
    }

    stream_entity<sizeof(__STREAM_T__), true> &get_entity() {
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
    ~directio() {
      bool vld = (size() != 0);
      if (vld)
      {
          std::cout << "WARNING [HLS SIM]: hls::directio '" 
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
    /// The valid function is only visible to mode::hs and mode::vld
    template<int M = __MODE__, typename std::enable_if<M == static_cast<int>(mode::hs) || M == static_cast<int>(mode::vld), int>::type = 0>
    bool valid() {
      return size() != 0;
    }    

    /// The ready function is only visible to mode::hs and mode::ack
    template<int M = __MODE__, typename std::enable_if<M == static_cast<int>(mode::hs) || M == static_cast<int>(mode::ack), int>::type = 0>
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
    /// The read_nb function is only visible to mode::hs and mode::vld
    template<int M = __MODE__, typename std::enable_if<M == static_cast<int>(mode::hs) || M == static_cast<int>(mode::vld), int>::type = 0>
    bool read_nb(__STREAM_T__& head) {
      bool vld = valid();
      if (vld)
          read(head); 
      return vld;
    }

    /// Nonblocking write
    /// The write_nb function is only visible to mode::hs and mode::ack
    template<int M = __MODE__, typename std::enable_if<M == static_cast<int>(mode::hs) || M == static_cast<int>(mode::ack), int>::type = 0>
    bool write_nb(const __STREAM_T__& tail) {
        bool rdy = ready();
        if (rdy)
            write(tail);
        return rdy;
    }

    void operator >> (__STREAM_T__& rdata) {
        read(rdata);
    }

    void operator << (const __STREAM_T__& wdata) {
        write(wdata);
    }


    /// Set name for c-sim debugging.
    void set_name(const char *name) { 
      get_entity().set_name(name);
    }

    void set_delegate(stream_delegate<sizeof(__STREAM_T__)> *d) {
      get_entity().d = d;
    }
};

template<typename __STREAM_T__>
using ap_hs = directio<__STREAM_T__, static_cast<int>(mode::hs)>;

template<typename __STREAM_T__>
using ap_vld = directio<__STREAM_T__, static_cast<int>(mode::vld)>;

template<typename __STREAM_T__>
using ap_ack = directio<__STREAM_T__, static_cast<int>(mode::ack)>;

template<typename __STREAM_T__>
using ap_none = directio<__STREAM_T__, static_cast<int>(mode::none)>;

} // namespace hls

#endif // __cplusplus
#endif  // X_HLS_DIRECTIO_SIM_H
