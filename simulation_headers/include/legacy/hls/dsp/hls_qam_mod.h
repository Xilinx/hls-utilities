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



#ifndef HLS_QAM_MOD_H
#define HLS_QAM_MOD_H

#include "ap_int.h"
#include <hls_stream.h>
#include <complex>
#include "legacy/hls/dsp/utils/hls_dsp_common_utils.h"

namespace hls {
#if (defined(OutputWidth) ||                    \
     defined(Constellation))
#error One or more of the template parameters used by this function is already defined and will cause a conflict
#endif

  // ===================================================================================================================
  // QAM MODULATOR: Class definition
  // o Template parameters:
  //  - Constellation : Selects the QAM modulation scheme to be used (QPSK, QAM16, etc)
  //  - OutputWidth   : Number of bits in each scalar component of the complex t_iq output.
  // The qam_mod block performs symbol to I/Q mapping as used in a QAM encoder. The output is zero extended up to the OutputWidth template parameter
  template<
    class Constellation,
    int OutputWidth>
    class qam_mod {

  public:

      typedef ap_int< OutputWidth > t_outcomponent;
      typedef std::complex< t_outcomponent > t_iq;

      qam_mod() {
        check_params();
      };//end of constructor

      ~qam_mod() {
      }; //end of destructor

      //Utilities
      //static function - owned by class not object since return value doesn't vary by object
      static int get_symbol_width() {
        return Constellation::SymbolWidth;
      }

      // ===================================================================================================================
      // QAM MODULATOR: Entry point function
      // o Arguments:
      //  - symbol        : type t_symbol. This is an N-bit symbol to be iq encoded. N is determined by the choice of QAM modulation scheme.
      //  - outputData    : type t_iq. This is a complex IQ output signal.
      void operator()(const typename Constellation::t_symbol &symbol,
                      t_iq &outputData) {

      Function_qam_mod_operator:;

        typename Constellation::t_star iq;

        Constellation::hardEncode(symbol,iq);
        outputData.real((t_outcomponent)iq.real() << (OutputWidth-Constellation::STAR_WIDTH));
        outputData.imag((t_outcomponent)iq.imag() << (OutputWidth-Constellation::STAR_WIDTH));
      } // operator()

  private:
      static const int MAX_OUTPUT_WIDTH = 16;

      Constellation asterism;

      void check_params() {
        // Verify that template parameters are correct in simulation
        #ifndef __SYNTHESIS__
        if (OutputWidth < Constellation::STAR_WIDTH || OutputWidth > MAX_OUTPUT_WIDTH) {
          std::cout << "ERROR: " << __FILE__ << ": OutputWidth must be between " << Constellation::STAR_WIDTH << " and " << MAX_OUTPUT_WIDTH << std::endl;
          exit(1);
        }
        #endif
      } //end of check_params

    }; // qam_mod

} //end namespace hls

#endif // HLS_QAM_MOD_H


