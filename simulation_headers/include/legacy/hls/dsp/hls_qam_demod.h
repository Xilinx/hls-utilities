// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689


#ifndef HLS_QAM_DEMOD_H
#define HLS_QAM_DEMOD_H

#include "ap_int.h"
#include <hls_stream.h>
#include <complex>
#include "legacy/hls/dsp/utils/hls_dsp_common_utils.h"

namespace hls {
#if (defined(InputWidth) ||                     \
     defined(Constellation))
#error One or more of the template parameters used by this function is already defined and will cause a conflict
#endif

  // ===================================================================================================================
  // QAM DEMODULATOR: Class definition
  // o Template parameters:
  //  - Constellation : Selects the QAM modulation scheme to be used (QPSK, QAM16, etc)
  //  - InputWidth    : Number of bits in each scalar component of the complex t_in input.
  // The qam_mod block performs I/Q to symbol mapping of a hard decision QAM decoder.
  template<
    class Constellation,
    int InputWidth>
    class qam_demod {

  public:

      typedef ap_int< InputWidth > t_incomponent;
      typedef std::complex< t_incomponent > t_in;

      qam_demod() {
        // Verify that template parameters are correct in simulation
        check_params();
      }; // end constructor

      ~qam_demod() {
      }; //end destructor

      //Utilities
      //static function - owned by class not object since return value doesn't vary by object
      static int get_symbol_width() {
        return Constellation::SYMBOL_WIDTH;
      }

      // ===================================================================================================================
      // QAM DEMODULATOR: Entry point function
      // o Arguments:
      //  - inputData     : type t_in. This is a complex IQ input signal.
      //  - symbol        : type t_symbol. This is an N-bit symbol determined as the closest match to the input IQ signal
      void operator()(const t_in &inputData,
                      typename Constellation::t_symbol &symbol ) {

      Function_qam_demod_operator:;

        //take leading bits of input data
        typename Constellation::t_hard_in hardIn;

        const int shift = InputWidth-Constellation::SYMBOL_WIDTH/2;

        hardIn.real(inputData.real() >> shift);
        hardIn.imag(inputData.imag() >> shift);

        Constellation::hardDecode(hardIn, symbol);

      } // operator()

  private:
      static const int MAX_INPUT_WIDTH = 16;

      void check_params() {
        // Verify that template parameters are correct in simulation
        #ifndef __SYNTHESIS__
        if (InputWidth > MAX_INPUT_WIDTH ) {
          std::cout << "ERROR: " << __FILE__ << ": InputWidth must be " << MAX_INPUT_WIDTH << " or less" << std::endl;
          exit(1);
        }
        if (InputWidth < Constellation::STAR_WIDTH && Constellation::QAMTYPE == 0) {
          std::cout << "ERROR: " << __FILE__ << ": InputWidth must be at least " << Constellation::STAR_WIDTH << " for QPSK" << std::endl;
          exit(1);
        }
        if (InputWidth < Constellation::STAR_WIDTH && Constellation::QAMTYPE == 1) {
          std::cout << "ERROR: " << __FILE__ << ": InputWidth must be at least " << Constellation::STAR_WIDTH << " for QAM16" << std::endl;
          exit(1);
        }
        if (InputWidth < Constellation::STAR_WIDTH && Constellation::QAMTYPE == 2) {
          std::cout << "ERROR: " << __FILE__ << ": InputWidth must be at least " << Constellation::STAR_WIDTH << " for QAM64" << std::endl;
          exit(1);
        }
        #endif
      } //end of check_params

    }; // qam_demod

} // end namespace hls

#endif // HLS_QAM_DEMOD_H


