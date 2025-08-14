// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef X_HLS_FFT_H
#define X_HLS_FFT_H

/*
 * This file contains a C++ model of hls::fft.
 * It defines Vivado_HLS synthesis model.
 */
#ifndef __cplusplus
#error C++ is required to include this header file
#else


#include <ap_int.h> 
#include <complex>
#include <cassert>
#include <hls_stream.h> 
#include <stdint.h> 
#ifndef __SYNTHESIS__
#include <math.h>
#endif

#ifndef AESL_SYN 
#include <iostream>
#include <fft/xfft_v9_1_bitacc_cmodel.h>
#endif

#ifndef __SYNTHESIS__
#include <iostream>
#include <fft/xfft_v9_1_bitacc_cmodel.h> 
#endif


namespace hls {

#ifdef AESL_SYN 
#include <etc/autopilot_ssdm_op.h>
#endif

enum fft_T0_t { inlined };
enum fft_T1_t { streamed };
enum fft_T2_t { nonblocking };
enum fft_T3_t { wrapped };

namespace ip_fft {

#ifdef __SYNTHESIS__
#ifndef AP_INLINE
#define AP_INLINE inline __attribute__((always_inline))
#endif
#else
#ifndef AP_INLINE
#define AP_INLINE inline
#endif
#endif // __SYNTHESIS__

static const char* fftErrChkHead = "ERROR:hls::fft ";

enum ordering {bit_reversed_order = 0, natural_order};
enum scaling {scaled = 0, unscaled, block_floating_point};
enum arch {
	radix_4_burst_io = 0, radix_2_burst_io,
	pipelined_streaming_io, radix_2_lite_burst_io
};
enum rounding {truncation = 0, convergent_rounding};
enum mem { block_ram = 0, distributed_ram };
enum opt {
	use_luts = 0, use_mults_resources,
	use_mults_performance, use_xtremedsp_slices
};
enum type { fixed_point = 0, floating_point, native_floating_point };
static const char* fft_data_format_str[] = {"fixed_point", "floating_point", "native_floating_point"};
enum ssr {ssr_1 = 1, ssr_2 = 2, ssr_4 = 4};

template<const int SSR_SAMPLE> struct data_item{
	std::complex<float> data[SSR_SAMPLE];
};

// TODO: add a checker to make ovflo = false when using native float

struct params_t
{
	static const unsigned input_width = 16;
	static const unsigned output_width = 16;
	static const unsigned status_width = 8;	
	static const unsigned config_width = 16;
	static const unsigned max_nfft = 10;

	static const bool has_nfft = false;
	static const unsigned channels = 1;
	static const unsigned arch_opt = pipelined_streaming_io;
	static const unsigned phase_factor_width = 16;
	static const unsigned ordering_opt = bit_reversed_order;
	static const bool ovflo = true;
	static const unsigned scaling_opt = scaled;
	static const unsigned rounding_opt = truncation;
	static const unsigned mem_data = block_ram;
	static const unsigned mem_phase_factors = block_ram;
	static const unsigned mem_reorder = block_ram;
	static const unsigned stages_block_ram = (max_nfft < 10) ? 0 : (max_nfft - 9);
	static const bool mem_hybrid = false;
	static const unsigned complex_mult_type = use_mults_resources;
	static const unsigned butterfly_type = use_luts;
	static const unsigned super_sample_rate = ssr_1;
	static const bool use_native_float = false;

	//not supported params:
	static const bool xk_index = false;
	static const bool cyclic_prefix_insertion = false;
};

template <typename CONFIG_T>
struct config_t
{
	config_t() {

	}

	ap_uint<CONFIG_T::config_width> data;
	// Check CONFIG_T::config_width
	AP_INLINE void checkBitWidth(ip_fft::type data_type = ip_fft::fixed_point)
	{
#ifndef AESL_SYN
		const unsigned max_nfft = CONFIG_T::max_nfft;
		const unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		const unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		const unsigned ch_bits = CONFIG_T::channels;
		const unsigned arch = CONFIG_T::arch_opt;
		const unsigned tmp_bits = (arch == unsigned(ip_fft::pipelined_streaming_io) || arch == unsigned(ip_fft::radix_4_burst_io)) ? ((max_nfft+1)>>1) * 2 : 2 * max_nfft;
		//Temporarily set floating point type to always generate scaling due to bugs in FFT IP
		const bool need_scaling = (data_type == ip_fft::floating_point) ? true : (CONFIG_T::scaling_opt == unsigned(ip_fft::scaled));
		const unsigned sch_bits = need_scaling ? tmp_bits : 0;
		const unsigned config_bits = (sch_bits + ch_bits) * CONFIG_T::channels + cp_len_bits + nfft_bits;
		const unsigned config_width = ((config_bits + 7) >> 3) << 3; // padding
		if (CONFIG_T::config_width != config_width)
		{
			std::cerr << ip_fft::fftErrChkHead << "Config channel width = " << (int)CONFIG_T::config_width
					<< " is illegal." << std::endl;
			std::cerr << "Correct width is " << config_width << ". Please refer to FFT IP in Vivado GUI for details" << std::endl;
			exit(1);
		}
#endif
	}

	AP_INLINE void checkCpLen(bool cp_len_enable)
	{
#ifndef AESL_SYN
		if (cp_len_enable == 0)
		{
			std::cerr << fftErrChkHead << "FFT_CYCLIC_PREFIX_INSERTION = false."
					<< " It's invalid to access cp_len field."
					<< std::endl;
			exit(1);
		}
#endif
	}

	AP_INLINE void checkSch(unsigned scaling_opt)
	{
#ifndef AESL_SYN
		if (scaling_opt != unsigned(scaled))
		{
			std::cerr << fftErrChkHead << "FFT_SCALING != scaled."
					<< " It's invalid to access scaling_sch field."
					<< std::endl;
			exit(1);
		}
#endif
	}

	AP_INLINE void setNfft(unsigned nfft)
	{
		//checkBitWidth();
		if (CONFIG_T::has_nfft) {
			data.range(7, 0) = nfft;
#ifndef AESL_SYN
			if (nfft > CONFIG_T::max_nfft)
				std::cerr << "Warning: the nfft value is larger than max_nfft. The behavior of the FFT is undefined." << std::endl;
#endif
		} else if (nfft != CONFIG_T::max_nfft) {
#ifndef AESL_SYN
			std::cerr << "Warning: As CONFIG_T::has_nfft is set to false, the nfft value cannot be changed. The max_nfft value is used in this case." << std::endl;
#endif
		}
	}
	AP_INLINE unsigned getNfft()
	{
		//checkBitWidth();
		if (CONFIG_T::has_nfft)
			return data.range(7, 0);
		else
			return CONFIG_T::max_nfft;
	}
	AP_INLINE unsigned getNfft() const
	{
		//checkBitWidth();
		if (CONFIG_T::has_nfft)
			return data.range(7, 0);
		else
			return CONFIG_T::max_nfft;
	}

	AP_INLINE void setCpLen(unsigned cp_len)
	{
		//checkBitWidth();
		checkCpLen(CONFIG_T::cyclic_prefix_insertion);
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		data.range(cp_len_bits+nfft_bits-1, nfft_bits) = cp_len;
	}
	AP_INLINE unsigned getCpLen()
	{
		//checkBitWidth();
		checkCpLen(CONFIG_T::cyclic_prefix_insertion);
		unsigned ret = 0;
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		ret = data.range(cp_len_bits+nfft_bits-1, nfft_bits);
		return ret;
	}
	AP_INLINE unsigned getCpLen() const
	{
		//checkBitWidth();
		checkCpLen(CONFIG_T::cyclic_prefix_insertion);
		unsigned ret = 0;
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		ret = data.range(cp_len_bits+nfft_bits-1, nfft_bits);
		return ret;
	}

	AP_INLINE void setDir(bool dir, unsigned ch = 0)
	{
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		unsigned ch_lo = cp_len_bits + nfft_bits;
		unsigned ch_bits = 1;
		data.range(ch_bits*(ch+1)+ch_lo-1, ch_bits*ch+ch_lo) = dir;
	}
	AP_INLINE unsigned getDir(unsigned ch = 0)
	{
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		unsigned ch_lo = cp_len_bits + nfft_bits;
		unsigned ch_bits = 1;
		return data.range(ch_bits*(ch+1)+ch_lo-1, ch_bits*ch+ch_lo);
	}
	AP_INLINE unsigned getDir(unsigned ch = 0) const
	{
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		unsigned ch_lo = cp_len_bits + nfft_bits;
		unsigned ch_bits = 1;
		return data.range(ch_bits*(ch+1)+ch_lo-1, ch_bits*ch+ch_lo);
	}

	AP_INLINE void setSch(unsigned sch, unsigned ch = 0)
	{
		//checkBitWidth();
		checkSch(CONFIG_T::scaling_opt);
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		unsigned ch_lo = cp_len_bits + nfft_bits;
		unsigned ch_bits = 1;
		unsigned arch = CONFIG_T::arch_opt;
		unsigned tmp_bits = (arch == unsigned(pipelined_streaming_io) || arch == unsigned(radix_4_burst_io)) ? ((max_nfft+1)>>1) * 2 : 2 * max_nfft;
		unsigned sch_bits = (CONFIG_T::scaling_opt == unsigned(scaled)) ? tmp_bits : 0;
		unsigned sch_lo = ch_lo + CONFIG_T::channels * ch_bits;
		data.range(sch_bits*(ch+1)+sch_lo-1, sch_bits*ch+sch_lo) = sch;
	}
	AP_INLINE unsigned getSch(unsigned ch = 0)
	{
		//checkBitWidth();
		checkSch(CONFIG_T::scaling_opt);
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		unsigned ch_lo = cp_len_bits + nfft_bits;
		unsigned ch_bits = 1;
		unsigned arch = CONFIG_T::arch_opt;
		unsigned tmp_bits = (arch == unsigned(pipelined_streaming_io) || arch == unsigned(radix_4_burst_io)) ? ((max_nfft+1)>>1) * 2 : 2 * max_nfft;
		unsigned sch_bits = (CONFIG_T::scaling_opt == unsigned(scaled)) ? tmp_bits : 0;
		unsigned sch_lo = ch_lo + CONFIG_T::channels * ch_bits;
		return data.range(sch_bits*(ch+1)+sch_lo-1, sch_bits*ch+sch_lo);
	}
	AP_INLINE unsigned getSch(unsigned ch = 0) const
	{
		//checkBitWidth();
		checkSch(CONFIG_T::scaling_opt);
		unsigned max_nfft = CONFIG_T::max_nfft;
		unsigned nfft_bits = CONFIG_T::has_nfft ? 8 : 0; // Padding to 8 bits
		unsigned cp_len_bits = CONFIG_T::cyclic_prefix_insertion ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
		unsigned ch_lo = cp_len_bits + nfft_bits;
		unsigned ch_bits = 1;
		unsigned arch = CONFIG_T::arch_opt;
		unsigned tmp_bits = (arch == unsigned(pipelined_streaming_io) || arch == unsigned(radix_4_burst_io)) ? ((max_nfft+1)>>1) * 2 : 2 * max_nfft;
		unsigned sch_bits = (CONFIG_T::scaling_opt == unsigned(scaled)) ? tmp_bits : 0;
		unsigned sch_lo = ch_lo + CONFIG_T::channels * ch_bits;
		return data.range(sch_bits*(ch+1)+sch_lo-1, sch_bits*ch+sch_lo);
	}
};

template<typename CONFIG_T>
struct status_t
{
	typedef ap_uint<CONFIG_T::status_width> status_data_t;
	status_data_t data;


	// Check CONFIG_T::status_width
	AP_INLINE void checkBitWidth()
	{
#ifndef AESL_SYN
		const bool has_ovflo = CONFIG_T::ovflo && (CONFIG_T::scaling_opt == unsigned(ip_fft::scaled));
		const unsigned blk_exp_bits = (CONFIG_T::scaling_opt == unsigned(ip_fft::block_floating_point)) ? 8 : 0; // padding to 8 bits
		const unsigned ovflo_bits = has_ovflo ? 1 : 0;
		const unsigned status_bits = (blk_exp_bits + ovflo_bits) * CONFIG_T::channels;
		const unsigned status_width = (status_bits == 0) ? 8 : ((status_bits + 7) >> 3) << 3; // padding
		if (CONFIG_T::status_width != status_width)
		{
			std::cerr << ip_fft::fftErrChkHead << "Status channel width = " << (int)CONFIG_T::status_width
					<< " is illegal." << std::endl;
			exit(1);
		}
#endif
	}

	AP_INLINE void checkBlkExp(unsigned scaling_opt)
	{
#ifndef AESL_SYN
		if (scaling_opt != unsigned(block_floating_point))
		{
			std::cerr << fftErrChkHead << "FFT_SCALING != block_floating_point."
					<< " It's invalid to access BLK_EXP field."
					<< std::endl;
			exit(1);
		}
#endif
	}

	AP_INLINE void checkOvflo(bool has_ovflo)
	{
#ifndef AESL_SYN
		if (!has_ovflo)
		{
			std::cerr << fftErrChkHead
					<< "Current configuration disables over flow field,"
					<< " it's invalid to access OVFLO field."
					<< std::endl;
			exit(1);
		}
#endif
	}

	AP_INLINE void setBlkExp(status_data_t exp)
	{
		checkBitWidth();
		checkBlkExp(CONFIG_T::scaling_opt);
		data = exp;
	}
	AP_INLINE unsigned getBlkExp(unsigned ch = 0)
	{
		checkBitWidth();
		unsigned blk_exp_bits = (CONFIG_T::scaling_opt == unsigned(block_floating_point)) ? 8 : 0; // padding to 8 bits
		checkBlkExp(CONFIG_T::scaling_opt);
		return data.range(blk_exp_bits*(ch+1)-1, blk_exp_bits*ch);
	}
	AP_INLINE unsigned getBlkExp(unsigned ch = 0) const
	{
		checkBitWidth();
		unsigned blk_exp_bits = (CONFIG_T::scaling_opt == unsigned(block_floating_point)) ? 8 : 0; // padding to 8 bits
		checkBlkExp(CONFIG_T::scaling_opt);
		return data.range(blk_exp_bits*(ch+1)-1, blk_exp_bits*ch);
	}

	AP_INLINE void setOvflo(status_data_t ovflo)
	{
		checkBitWidth();
		bool has_ovflo = CONFIG_T::ovflo && (CONFIG_T::scaling_opt == unsigned(scaled));
		checkOvflo(has_ovflo);
		data = ovflo;
	}
	AP_INLINE unsigned getOvflo(unsigned ch = 0)
	{
		checkBitWidth();
		bool has_ovflo = CONFIG_T::ovflo && (CONFIG_T::scaling_opt == unsigned(scaled));
		unsigned ovflo_bits = has_ovflo ? 1 : 0;
		checkOvflo(has_ovflo);
		return data.range(ovflo_bits*(ch+1)-1, ovflo_bits*ch);
	}
	AP_INLINE unsigned getOvflo(unsigned ch = 0) const
	{
		checkBitWidth();
		bool has_ovflo = CONFIG_T::ovflo && (CONFIG_T::scaling_opt == unsigned(scaled));
		unsigned ovflo_bits = has_ovflo ? 1 : 0;
		checkOvflo(has_ovflo);
		return data.range(ovflo_bits*(ch+1)-1, ovflo_bits*ch);
	}
};

} // namespace hls::ip_fft

template<
typename CONFIG_T,
char FFT_INPUT_WIDTH,
char FFT_OUTPUT_WIDTH,
typename FFT_INPUT_T,
typename FFT_OUTPUT_T,
int FFT_LENGTH,
char FFT_CHANNELS,
ip_fft::type FFT_DATA_FORMAT,
char FFT_super_sample_rate
>
AP_INLINE void fft_core(
		std::complex<FFT_INPUT_T> xn[FFT_CHANNELS][FFT_LENGTH],
		std::complex<FFT_OUTPUT_T> xk[FFT_CHANNELS][FFT_LENGTH],
		ip_fft::status_t<CONFIG_T>* status,
		ip_fft::config_t<CONFIG_T>* config_ch)
{

#ifdef AESL_SYN

//////////////////////////////////////////////
// C level synthesis models for hls::fft
	//////////////////////////////////////////////

#pragma HLS inline
	__fpga_ip("Vivado_FFT",
			//"component_name", "xfft_0",
			"channels", FFT_CHANNELS,
			"transform_length", 1 << CONFIG_T::max_nfft,
			"implementation_options", CONFIG_T::arch_opt,
			"run_time_configurable_transform_length", CONFIG_T::has_nfft,
			"data_format", ip_fft::fft_data_format_str[FFT_DATA_FORMAT],
			"input_width", FFT_INPUT_WIDTH,
			"output_width", FFT_OUTPUT_WIDTH,
			"phase_factor_width", CONFIG_T::phase_factor_width,
			"scaling_options", CONFIG_T::scaling_opt,
			"rounding_modes", CONFIG_T::rounding_opt,
			"aclken", "true",
			"aresetn", "true",
			"ovflo", CONFIG_T::ovflo,
			"xk_index", CONFIG_T::xk_index,
			"throttle_scheme", "nonrealtime",
			"output_ordering", CONFIG_T::ordering_opt,
			"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
			"memory_options_data", CONFIG_T::mem_data,
			"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
			"memory_options_reorder", CONFIG_T::mem_reorder,
			"number_of_stages_using_block_ram_for_data_and_phase_factors",
			CONFIG_T::stages_block_ram,
			"memory_options_hybrid", CONFIG_T::mem_hybrid,
			"complex_mult_type", CONFIG_T::complex_mult_type,
			"butterfly_type", CONFIG_T::butterfly_type,
			"super_sample_rates", FFT_super_sample_rate
	);
	ip_fft::status_t<CONFIG_T> status_t;
	ip_fft::config_t<CONFIG_T> config_ch_t = *config_ch;

	bool has_scaling_sch =  config_ch_t.getSch();
	bool has_direction = config_ch_t.getDir();

	if ( has_direction || has_scaling_sch )
		for (int i = 0; i < FFT_LENGTH; ++i)
		{
			for (int c = 0; c < FFT_CHANNELS; ++c)
			{
#pragma HLS unroll 
				xk[c][i] = xn[c][i];
			}
		}

	status_t.data = config_ch_t.getDir();
	*status = status_t;
#else

	//////////////////////////////////////////////
	// C level simulation models for hls::fft
	//////////////////////////////////////////////

	// Declare the C model IO structures
	xilinx_ip_xfft_v9_1_generics  generics;
	xilinx_ip_xfft_v9_1_state    *state;
	xilinx_ip_xfft_v9_1_inputs    inputs;
	xilinx_ip_xfft_v9_1_outputs   outputs;

	// Log2 of FFT length
	int fft_length = FFT_LENGTH;
	int NFFT = 0;
	if (CONFIG_T::has_nfft)
		NFFT = config_ch->getNfft();
	else
		NFFT = CONFIG_T::max_nfft;

	const int samples =  1 << NFFT;

	///////////// IP parameters legality checking /////////////

	// Check CONFIG_T::config_width
	config_ch->checkBitWidth(FFT_DATA_FORMAT);

	// Check CONFIG_T::status_width
	status->checkBitWidth();

	// Check ip parameters
	if (CONFIG_T::channels < 1 || CONFIG_T::channels > 12)
	{
		std::cerr << ip_fft::fftErrChkHead << "Channels = " << (int)CONFIG_T::channels
				<< " is illegal. It should be from 1 to 12."
				<< std::endl;
		exit(1);
	}

	if (CONFIG_T::max_nfft < 3 || CONFIG_T::max_nfft > 16)
	{
		std::cerr << ip_fft::fftErrChkHead << "NFFT_MAX = " << (int)CONFIG_T::max_nfft
				<< " is illegal. It should be from 3 to 16."
				<< std::endl;
		exit(1);
	}

	unsigned length = FFT_LENGTH;
	if (!CONFIG_T::has_nfft)
	{
		if (FFT_LENGTH != (1 << CONFIG_T::max_nfft))
		{
			std::cerr << ip_fft::fftErrChkHead << "FFT_LENGTH = " << (int)FFT_LENGTH
					<< " is illegal. Log2(FFT_LENGTH) should equal to NFFT_MAX when run-time configurable length is disabled."
					<< std::endl;
			exit(1);
		}
	}
	else if (length & (length - 1))
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_LENGTH = " << (int)FFT_LENGTH
				<< " is illegal. It should be the integer power of 2."
				<< std::endl;
		exit(1);
	}
	else if (NFFT < 3 || NFFT > 16)
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_LENGTH = " << (int)FFT_LENGTH
				<< " is illegal. Log2(FFT_LENGTH) should be from 3 to 16."
				<< std::endl;
		exit(1);
	}
	else if (NFFT > CONFIG_T::max_nfft)
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_LENGTH = " << (int)FFT_LENGTH
				<< " is illegal. Log2(FFT_LENGTH) should be less than or equal to NFFT_MAX."
				<< std::endl;
		exit(1);
	}
#if 0
	else if (NFFT != config_ch->getNfft())
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_LENGTH = " << (int)FFT_LENGTH
				<< " is illegal. Log2(FFT_LENGTH) should equal to NFFT field of configure channel."
				<< std::endl;
		exit(1);
	}
#endif

	if ((FFT_INPUT_WIDTH < 8) || (FFT_INPUT_WIDTH > 40))
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_INPUT_WIDTH = " << (int)FFT_INPUT_WIDTH
				<< " is illegal. It should be 8,16,24,32,40."
				<< std::endl;
		exit(1);
	}

	if (CONFIG_T::scaling_opt == ip_fft::unscaled && FFT_DATA_FORMAT != ip_fft::floating_point)
	{
		unsigned golden = FFT_INPUT_WIDTH + CONFIG_T::max_nfft + 1;
		golden = ((golden + 7) >> 3) << 3;
		if (FFT_OUTPUT_WIDTH != golden)
		{
			std::cerr << ip_fft::fftErrChkHead << "FFT_OUTPUT_WIDTH = " << (int)FFT_OUTPUT_WIDTH
					<< " is illegal with unscaled arithmetic. It should be input_width+nfft_max+1."
					<< std::endl;
			exit(1);
		}
	}
	else if (FFT_OUTPUT_WIDTH != FFT_INPUT_WIDTH)
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_OUTPUT_WIDTH = " << (int)FFT_OUTPUT_WIDTH
				<< " is illegal. It should be the same as input_width."
				<< std::endl;
		exit(1);
	}

	if (CONFIG_T::channels > 1 && CONFIG_T::arch_opt == ip_fft::pipelined_streaming_io)
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_CHANNELS = " << (int)CONFIG_T::channels << " and FFT_ARCH = pipelined_streaming_io"
				<< " is illegal. pipelined_streaming_io architecture is not supported when channels is bigger than 1."
				<< std::endl;
		exit(1);
	}

	if (CONFIG_T::channels > 1 && FFT_DATA_FORMAT == ip_fft::floating_point)
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_CHANNELS = " << (int)CONFIG_T::channels
				<< " is illegal with floating point data format. Floating point data format only supports 1 channel."
				<< std::endl;
		exit(1);
	}

	if(CONFIG_T::super_sample_rate != ip_fft::ssr_1){
		if(FFT_DATA_FORMAT != ip_fft::floating_point){
			std::cerr << ip_fft::fftErrChkHead << "FFT_SUPER_SAMPLE_RATE = " << (int)CONFIG_T::super_sample_rate
					<< " is illegal with fixed point data format. Should use floating type."
					<< std::endl;
			exit(1);
		}
		if(!CONFIG_T::use_native_float){
			std::cerr << ip_fft::fftErrChkHead << "FFT_SUPER_SAMPLE_RATE = " << (int)CONFIG_T::super_sample_rate
					<< " should be used with setting use_native_float=1."
					<< std::endl;
			exit(1);
		}
	}

	if (FFT_DATA_FORMAT == ip_fft::floating_point)
	{
		if(!CONFIG_T::use_native_float){
			if (CONFIG_T::phase_factor_width != 24 && CONFIG_T::phase_factor_width != 25)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_PHASE_FACTOR_WIDTH = " << (int)CONFIG_T::phase_factor_width
						<< " is illegal with floating point data format. It should be 24 or 25."
						<< std::endl;
				exit(1);
			}
		}
		else {
			// native float specific checks
			if(CONFIG_T::has_nfft != 0)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_HAS_NFFT = " << (int)CONFIG_T::has_nfft
						<< " is illegal with native floating point data format. It should be 0."
						<< std::endl;
				exit(1);
			}
			if(CONFIG_T::phase_factor_width != 32)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_PHASE_FACTOR_WIDTH = " << (int)CONFIG_T::phase_factor_width
						<< " is illegal with native floating point data format. It should be 32."
						<< std::endl;
				exit(1);
			}
			if(CONFIG_T::ordering_opt != ip_fft::natural_order)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_ORDERING_OPT = bit_reversed_order" 
						<< " is illegal with native floating point data format. It should be natural_order."
						<< std::endl;
				exit(1);
			}
			if(CONFIG_T::ovflo != false)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_OVFLO = " << CONFIG_T::ovflo
						<< " is illegal with native floating point data format. It should be false."
						<< std::endl;
				exit(1);
			}
			if(CONFIG_T::complex_mult_type != ip_fft::use_mults_performance)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_COMPLEX_MULT_TYPE"
						<< " is illegal with native floating point data format. It should be use_mults_performance."
						<< std::endl;
				exit(1);
			}
			if(CONFIG_T::butterfly_type != ip_fft::use_xtremedsp_slices)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_BUTTERFLY_TYPE"
						<< " is illegal with native floating point data format. It should be use_xtremedsp_slices."
						<< std::endl;
				exit(1);
			}
			if(CONFIG_T::super_sample_rate == ip_fft::ssr_1)
			{
				std::cerr << ip_fft::fftErrChkHead << "FFT_SUPER_SAMPLE_RATE = 1"
						<< " is illegal with native floating point data format. It should be 2 or 4."
						<< std::endl;
				exit(1);
			}
		}
	}
	else if (CONFIG_T::phase_factor_width < 8 || CONFIG_T::phase_factor_width > 34)
	{
		std::cerr << ip_fft::fftErrChkHead << "FFT_PHASE_FACTOR_WIDTH = " << (int)CONFIG_T::phase_factor_width
				<< " is illegal. It should be from 8 to 34."
				<< std::endl;
		exit(1);
	}

	//////////////////////////////////////////////////////////

	// Build up the C model generics structure
	generics.C_NFFT_MAX      = CONFIG_T::max_nfft;
	generics.C_ARCH          = CONFIG_T::arch_opt+1;
	generics.C_HAS_NFFT      = CONFIG_T::has_nfft;
	generics.C_INPUT_WIDTH   = FFT_INPUT_WIDTH;
	generics.C_TWIDDLE_WIDTH = CONFIG_T::phase_factor_width;
	generics.C_HAS_SCALING   = CONFIG_T::scaling_opt == ip_fft::unscaled ? 0 : 1;
	generics.C_HAS_BFP       = CONFIG_T::scaling_opt == ip_fft::block_floating_point ? 1 : 0;
	generics.C_HAS_ROUNDING  = CONFIG_T::rounding_opt;
	generics.C_USE_FLT_PT    = FFT_DATA_FORMAT == ip_fft::floating_point ? (CONFIG_T::use_native_float)? 2 : 1 : 0;
	generics.C_NSSR 		 = FFT_super_sample_rate;
	generics.C_SYSTOLICFFT_INV = false;

	// Create an FFT state object
	state = xilinx_ip_xfft_v9_1_create_state(generics);

	int stages = 0;
	if ((generics.C_ARCH == 2) || (generics.C_ARCH == 4))  // radix-2
		stages = NFFT;
	else  // radix-4 or radix-22
		stages = (NFFT+1)/2;

	double* xn_re       = (double*) malloc(samples * sizeof(double));
	double* xn_im       = (double*) malloc(samples * sizeof(double));
	int*    scaling_sch = (int*)    malloc(stages  * sizeof(int));
	double* xk_re       = (double*) malloc(samples * sizeof(double));
	double* xk_im       = (double*) malloc(samples * sizeof(double));

	// Check the memory was allocated successfully for all arrays
	if (xn_re == NULL || xn_im == NULL || scaling_sch == NULL || xk_re == NULL || xk_im == NULL)
	{
		std::cerr << "Couldn't allocate memory for input and output data arrays - dying" << std::endl;
		exit(3);
	}

	ap_uint<CONFIG_T::status_width> overflow = 0;
	ap_uint<CONFIG_T::status_width> blkexp = 0;
	for (int c = 0; c < FFT_CHANNELS; ++c)
	{
		// Set pointers in input and output structures
		inputs.xn_re       = xn_re;
		inputs.xn_im       = xn_im;
		inputs.scaling_sch = scaling_sch;
		outputs.xk_re      = xk_re;
		outputs.xk_im      = xk_im;

		// Store in inputs structure
		inputs.nfft = NFFT;
		// config data
		inputs.direction = config_ch->getDir(c);
		unsigned scaling = 0;
		if (CONFIG_T::scaling_opt == ip_fft::scaled)
			scaling = config_ch->getSch(c);
		for (int i = 0; i < stages; i++)
		{
			inputs.scaling_sch[i] = scaling & 0x3;
			scaling >>= 2;
		}
		inputs.scaling_sch_size = stages;
		for (int i = 0; i < samples ; i++)
		{
			std::complex<FFT_INPUT_T> din = xn[c][i];
			inputs.xn_re[i] = (double)din.real();
			inputs.xn_im[i] = (double)din.imag();
#ifdef _HLSCLIB_DEBUG_
std::cout << "xn[" << c << "][" << i << ": xn_re = " << inputs .xn_re[i] <<
		" xk_im = " <<  inputs.xn_im[i] << endl;
#endif
		}
		inputs.xn_re_size = samples;
		inputs.xn_im_size = samples;

		// Set sizes of output structure arrays
		outputs.xk_re_size    = samples;
		outputs.xk_im_size    = samples;

		//#define DEBUG
#ifdef _HLSCLIB_DEBUG_
		///////////////////////////////////////////////////////////////////////////////
		/// Debug
		std::cout << "About to call the C model with:" << std::endl;
		std::cout << "Generics:" << std::endl;
		std::cout << "  C_NFFT_MAX = "        << generics.C_NFFT_MAX << std::endl;
		std::cout << "  C_ARCH = "            << generics.C_ARCH << std::endl;
		std::cout << "  C_HAS_NFFT = "        << generics.C_HAS_NFFT << std::endl;
		std::cout << "  C_INPUT_WIDTH = "     << generics.C_INPUT_WIDTH << std::endl;
		std::cout << "  C_TWIDDLE_WIDTH = "   << generics.C_TWIDDLE_WIDTH << std::endl;
		std::cout << "  C_HAS_SCALING = "     << generics.C_HAS_SCALING << std::endl;
		std::cout << "  C_HAS_BFP = "         << generics.C_HAS_BFP << std::endl;
		std::cout << "  C_HAS_ROUNDING = "    << generics.C_HAS_ROUNDING << std::endl;
		std::cout << "  C_USE_FLT_PT = "      << generics.C_USE_FLT_PT << std::endl;
		std::cout << "  C_NSSR = "            << generics.C_NSSR << std::endl;
		std::cout << "  C_SYSTOLICFFT_INV = " << generics.C_SYSTOLICFFT_INV << std::endl;

		std::cout << "Inputs structure:" << std::endl;
		std::cout << "  nfft = " << inputs.nfft << std::endl;
		printf("  xn_re[0] = %e\n",inputs.xn_re[0]);
		std::cout << "  xn_re_size = " << inputs.xn_re_size << std::endl;
		printf("  xn_im[0] = %e\n",inputs.xn_im[0]);
		std::cout << "  xn_im_size = " << inputs.xn_im_size << std::endl;

		for (int i = stages - 1; i >= 0; --i)
			std::cout << "  scaling_sch[" << i << "] = " << inputs.scaling_sch[i] << std::endl;

		std::cout << "  scaling_sch_size = " << inputs.scaling_sch_size << std::endl;
		std::cout << "  direction = " << inputs.direction << std::endl;

		std::cout << "Outputs structure:" << std::endl;
		std::cout << "  xk_re_size = " << outputs.xk_re_size << std::endl;
		std::cout << "  xk_im_size = " << outputs.xk_im_size << std::endl;

		// Run the C model to generate output data
		std::cout << "Running the C model..." << std::endl;
		///////////////////////////////////////////////////////////////////////////////
#endif

		int result = 0;
		result = xilinx_ip_xfft_v9_1_bitacc_simulate(state, inputs, &outputs);
		if (result != 0)
		{
			std::cerr << "An error occurred when simulating the FFT core: return code " << result << std::endl;
			exit(4);
		}

		// Output data
		for (int i = 0; i < samples; i++)
		{
			std::complex<FFT_OUTPUT_T> dout;
			unsigned addr_reverse = 0;
			for (int k = 0; k < NFFT; ++k)
			{
				addr_reverse <<= 1;
				addr_reverse |= (i >> k) & 0x1;
			}
			unsigned addr = i;
			if (CONFIG_T::ordering_opt == ip_fft::bit_reversed_order)
				addr = addr_reverse;
			dout = std::complex<FFT_OUTPUT_T> (outputs.xk_re[addr], outputs.xk_im[addr]);
			xk[c][i] = dout;
#ifdef _HLSCLIB_DEBUG_
cout << "xk[" << c "][" << i << ": xk_re = " << outputs.xk_re[addr] <<
		" xk_im = " <<  outputs.xk_im[addr] << endl;
#endif
		}

		// Status
		if (CONFIG_T::scaling_opt == ip_fft::block_floating_point)
			blkexp.range(c*8+7, c*8) = outputs.blk_exp;
		else if (CONFIG_T::ovflo && (CONFIG_T::scaling_opt == ip_fft::scaled))
			overflow.range(c, c) = outputs.overflow;
	}

	// Status
	if (CONFIG_T::scaling_opt == ip_fft::block_floating_point)
		status->setBlkExp(blkexp);
	else if (CONFIG_T::ovflo && (CONFIG_T::scaling_opt == ip_fft::scaled))
		status->setOvflo(overflow);

	// Release memory used for input and output arrays
	free(xn_re);
	free(xn_im);
	free(scaling_sch);
	free(xk_re);
	free(xk_im);

	// Destroy FFT state to free up memory
	xilinx_ip_xfft_v9_1_destroy_state(state);
#endif

} // End of fft_core


template<
typename CONFIG_T,
char FFT_INPUT_WIDTH,
char FFT_OUTPUT_WIDTH,
typename FFT_INPUT_T,
typename FFT_OUTPUT_T,
int FFT_LENGTH,
char FFT_CHANNELS,
ip_fft::type FFT_DATA_FORMAT,
char FFT_super_sample_rate
>
void fft_core(
		std::complex<FFT_INPUT_T> xn[FFT_LENGTH],
		std::complex<FFT_OUTPUT_T> xk[FFT_LENGTH],
		ip_fft::status_t<CONFIG_T>* status,
		ip_fft::config_t<CONFIG_T>* config_ch)
{
#ifdef AESL_SYN
#pragma HLS inline

	__fpga_ip("Vivado_FFT",
			//"component_name", "xfft_0",
			"channels", FFT_CHANNELS,
			"transform_length", FFT_LENGTH,
			"implementation_options", CONFIG_T::arch_opt,
			"run_time_configurable_transform_length", CONFIG_T::has_nfft,
			"data_format", ip_fft::fft_data_format_str[FFT_DATA_FORMAT],
			"input_width", FFT_INPUT_WIDTH,
			"output_width", FFT_OUTPUT_WIDTH,
			"phase_factor_width", CONFIG_T::phase_factor_width,
			"scaling_options", CONFIG_T::scaling_opt,
			"rounding_modes", CONFIG_T::rounding_opt,
			"aclken", "true",
			"aresetn", "true",
			"ovflo", CONFIG_T::ovflo,
			"xk_index", CONFIG_T::xk_index,
			"throttle_scheme", "nonrealtime",
			"output_ordering", CONFIG_T::ordering_opt,
			"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
			"memory_options_data", CONFIG_T::mem_data,
			"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
			"memory_options_reorder", CONFIG_T::mem_reorder,
			"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
			"memory_options_hybrid", CONFIG_T::mem_hybrid,
			"complex_mult_type", CONFIG_T::complex_mult_type,
			"butterfly_type", CONFIG_T::butterfly_type,
			"super_sample_rates", FFT_super_sample_rate
	);
	ip_fft::status_t<CONFIG_T> status_t;
	ip_fft::config_t<CONFIG_T> config_ch_t = *config_ch;

	bool has_scaling_sch =  config_ch_t.getSch();
	bool has_direction = config_ch_t.getDir();

	if ( has_direction || has_scaling_sch )
		for (int i = 0; i < FFT_LENGTH; ++i)
		{
			xk[i] = xn[i];
		}

	status_t.data = config_ch_t.getDir();
	*status = status_t;

#else
	std::complex<FFT_INPUT_T> xn_multi_chan [1][FFT_LENGTH];
	std::complex<FFT_OUTPUT_T> xk_multi_chan [1][FFT_LENGTH];

	for(int i=0; i< FFT_LENGTH; i++)
		xn_multi_chan[0][i] = xn[i];

	fft_core<
	CONFIG_T,
	FFT_INPUT_WIDTH,
	FFT_OUTPUT_WIDTH,
	FFT_INPUT_T,
	FFT_OUTPUT_T,
	FFT_LENGTH,
	1,
	FFT_DATA_FORMAT,
	FFT_super_sample_rate
	>(xn_multi_chan, xk_multi_chan, status, config_ch);

	for(int i=0; i< FFT_LENGTH; i++)
		xk[i] = xk_multi_chan[0][i];
#endif
}


#ifdef __SYNTHESIS__
// 1-channel, fixed-point, streaming
template<
typename CONFIG_T
>
void fft_syn(
		hls::stream<std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > > &xn,
		hls::stream<std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > > &xk,
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V)
{
#pragma HLS inline  

	__fpga_ip("Vivado_FFT",
			//"component_name", "xfft_0",
			"channels", 1,
			"transform_length", 1 << CONFIG_T::max_nfft,
			"implementation_options", CONFIG_T::arch_opt,
			"run_time_configurable_transform_length", CONFIG_T::has_nfft,
			"data_format", ip_fft::fft_data_format_str[ip_fft::fixed_point],
			"input_width", CONFIG_T::input_width,
			"output_width", CONFIG_T::output_width,
			"phase_factor_width", CONFIG_T::phase_factor_width,
			"scaling_options", CONFIG_T::scaling_opt,
			"rounding_modes", CONFIG_T::rounding_opt,
			"aclken", "true",
			"aresetn", "true",
			"ovflo", CONFIG_T::ovflo,
			"xk_index", CONFIG_T::xk_index,
			"throttle_scheme", "nonrealtime",
			"output_ordering", CONFIG_T::ordering_opt,
			"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
			"memory_options_data", CONFIG_T::mem_data,
			"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
			"memory_options_reorder", CONFIG_T::mem_reorder,
			"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
			"memory_options_hybrid", CONFIG_T::mem_hybrid,
			"complex_mult_type", CONFIG_T::complex_mult_type,
			"butterfly_type", CONFIG_T::butterfly_type,
			"super_sample_rates", CONFIG_T::super_sample_rate
	);


	ip_fft::config_t<CONFIG_T> config_tmp = config_ch_data_V.read();
	bool has_scaling_sch =  config_tmp.getSch();
	bool has_direction = config_tmp.getDir();
	int FFT_LENGTH = 1 << CONFIG_T::max_nfft;
	if ( has_direction || has_scaling_sch )
		for (int i = 0; i < FFT_LENGTH; ++i)
		{
			xk.write(xn.read());
		}

	ip_fft::status_t<CONFIG_T> status_tmp;
	status_tmp.data = config_tmp.getDir();
	status_data_V.write(status_tmp);

} // End of 1-channel, fixed-point, streaming

#endif

#ifdef __SYNTHESIS__
// 1-channel, fixed-point, partial streaming
template<
typename CONFIG_T
>
void fft_syn(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V)
{
#pragma HLS inline off

	__fpga_ip("Vivado_FFT",
			//"component_name", "xfft_0",
			"channels", 1,
			"transform_length", 1 << CONFIG_T::max_nfft,
			"implementation_options", CONFIG_T::arch_opt,
			"run_time_configurable_transform_length", CONFIG_T::has_nfft,
			"data_format", ip_fft::fft_data_format_str[ip_fft::fixed_point],
			"input_width", CONFIG_T::input_width,
			"output_width", CONFIG_T::output_width,
			"phase_factor_width", CONFIG_T::phase_factor_width,
			"scaling_options", CONFIG_T::scaling_opt,
			"rounding_modes", CONFIG_T::rounding_opt,
			"aclken", "true",
			"aresetn", "true",
			"ovflo", CONFIG_T::ovflo,
			"xk_index", CONFIG_T::xk_index,
			"throttle_scheme", "nonrealtime",
			"output_ordering", CONFIG_T::ordering_opt,
			"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
			"memory_options_data", CONFIG_T::mem_data,
			"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
			"memory_options_reorder", CONFIG_T::mem_reorder,
			"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
			"memory_options_hybrid", CONFIG_T::mem_hybrid,
			"complex_mult_type", CONFIG_T::complex_mult_type,
			"butterfly_type", CONFIG_T::butterfly_type,
			"super_sample_rates", CONFIG_T::super_sample_rate
	);


	ip_fft::config_t<CONFIG_T> config_tmp = config_ch_data_V.read();
	bool has_scaling_sch =  config_tmp.getSch();
	bool has_direction = config_tmp.getDir();
	int FFT_LENGTH = 1 << CONFIG_T::max_nfft;
	if ( has_direction || has_scaling_sch )
		for (int i = 0; i < FFT_LENGTH; ++i)
		{
			xk[i] = xn[i];
		}

	ip_fft::status_t<CONFIG_T> status_tmp;
	status_tmp.data = config_tmp.getDir();
	status_data_V.write(status_tmp);

} // End of 1-channel, fixed-point, partial streaming
#endif

#ifdef __SYNTHESIS__
// multi-channels, fixed-point, partial streaming
template<
typename CONFIG_T
>
void fft_syn(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V)
{
#pragma HLS inline off

	__fpga_ip("Vivado_FFT",
			//"component_name", "xfft_0",
			"channels", CONFIG_T::channels,
			"transform_length", 1 << CONFIG_T::max_nfft,
			"implementation_options", CONFIG_T::arch_opt,
			"run_time_configurable_transform_length", CONFIG_T::has_nfft,
			"data_format", ip_fft::fft_data_format_str[ip_fft::fixed_point],
			"input_width", CONFIG_T::input_width,
			"output_width", CONFIG_T::output_width,
			"phase_factor_width", CONFIG_T::phase_factor_width,
			"scaling_options", CONFIG_T::scaling_opt,
			"rounding_modes", CONFIG_T::rounding_opt,
			"aclken", "true",
			"aresetn", "true",
			"ovflo", CONFIG_T::ovflo,
			"xk_index", CONFIG_T::xk_index,
			"throttle_scheme", "nonrealtime",
			"output_ordering", CONFIG_T::ordering_opt,
			"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
			"memory_options_data", CONFIG_T::mem_data,
			"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
			"memory_options_reorder", CONFIG_T::mem_reorder,
			"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
			"memory_options_hybrid", CONFIG_T::mem_hybrid,
			"complex_mult_type", CONFIG_T::complex_mult_type,
			"butterfly_type", CONFIG_T::butterfly_type,
			"super_sample_rates", CONFIG_T::super_sample_rate
	);

	ip_fft::config_t<CONFIG_T> config_tmp = config_ch_data_V.read();
	bool has_scaling_sch =  config_tmp.getSch();
	bool has_direction = config_tmp.getDir();
	int FFT_LENGTH = 1 << CONFIG_T::max_nfft;
	int FFT_CHANNELS = CONFIG_T::channels;
	if ( has_direction || has_scaling_sch )
		for (int i = 0; i < FFT_LENGTH; ++i)
		{
			for (int c = 0; c < FFT_CHANNELS; ++c)
			{
#pragma HLS unroll 
				xk[c][i] = xn[c][i];
			}
		}

	ip_fft::status_t<CONFIG_T> status_tmp;
	status_tmp.data = config_tmp.getDir();
	status_data_V.write(status_tmp);

} // End of multi-channels, fixed-point, partial streaming
#endif

// 1-channel, fixed-point
template<
typename CONFIG_T
>
void fft_sim(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch)
{
	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	1,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, status, config_ch);
} // End of 1-channel, fixed-point

template<
typename CONFIG_T
>
void data_copy_from_ap_fix_to_ap_uint(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		ap_uint<((CONFIG_T::input_width+7)/8)*8*2> xn_cp[1 << CONFIG_T::max_nfft]) {

	for (unsigned i = 0; i < (1 << CONFIG_T::max_nfft); i++) {
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn_tmp = xn[i];
		ap_uint<((CONFIG_T::input_width+7)/8)*8*2> xn_cp_tmp;
		xn_cp_tmp(((CONFIG_T::input_width+7)/8)*8 - 1, 0) = xn_tmp.real().range(((CONFIG_T::input_width+7)/8)*8 - 1, 0);
		xn_cp_tmp(((CONFIG_T::input_width+7)/8)*8*2 - 1, ((CONFIG_T::input_width+7)/8)*8) = xn_tmp.imag().range(((CONFIG_T::input_width+7)/8)*8 - 1, 0);
		xn_cp[i] = xn_cp_tmp;
	}
}

template<
typename CONFIG_T
>
void data_copy_from_ap_uint_to_ap_fixed(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xk[1 << CONFIG_T::max_nfft],
		ap_uint<((CONFIG_T::input_width+7)/8)*8*2> xk_cp[1 << CONFIG_T::max_nfft]) {
	for (unsigned i = 0; i < (1 << CONFIG_T::max_nfft); i++) {
		ap_uint<((CONFIG_T::input_width+7)/8)*8*2> xk_cp_tmp = xk_cp[i];
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, 1> > xk_tmp;
		ap_fixed<((CONFIG_T::output_width+7)/8)*8, 1> tmp;
		tmp.range(((CONFIG_T::output_width+7)/8)*8 - 1, 0) = xk_cp_tmp.range(((CONFIG_T::output_width+7)/8)*8 - 1, 0);
		xk_tmp.real(tmp);
		tmp.range(((CONFIG_T::output_width+7)/8)*8 - 1, 0) = xk_cp_tmp.range(((CONFIG_T::output_width+7)/8)*8*2 - 1, ((CONFIG_T::output_width+7)/8)*8);
		xk_tmp.imag(tmp);
		xk[i] = xk_tmp;
	}
}

// 1-channel, fixed-point, streaming, not inlined
template<
typename CONFIG_T
>
void  fft(
		hls::stream<std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > > &xn,
		hls::stream<std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > > &xk,
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V)
{
#ifdef __SYNTHESIS__
#pragma HLS inline off

	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // inlined
#else
	std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn_a[1 << CONFIG_T::max_nfft];
	std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk_a[1 << CONFIG_T::max_nfft];
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read(config);
	unsigned int bound = CONFIG_T::has_nfft ? config.getNfft() : CONFIG_T::max_nfft;
	for (unsigned int i = 0; i < 1 << bound; i++)
		xn_a[i] = xn.read();
	fft_sim<CONFIG_T>(xn_a, xk_a, &status, &config);
	for (unsigned int i = 0; i < 1 << bound; i++)
		xk.write(xk_a[i]);
	status_data_V.write(status);
#endif
}

// 1-channel, fixed-point, streaming, non-blocking for simulation
template<
typename CONFIG_T, fft_T2_t tag
>
void  fft(
		hls::stream<std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > > &xn,
		hls::stream<std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > > &xk,
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V)
{
#ifdef __SYNTHESIS__
#pragma HLS inline off

	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // inlined
#else
	std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn_a[1 << CONFIG_T::max_nfft];
	std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk_a[1 << CONFIG_T::max_nfft];
	ip_fft::status_t<CONFIG_T> status;
	static ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read_nb(config);
	unsigned int bound = CONFIG_T::has_nfft ? config.getNfft() : CONFIG_T::max_nfft;
	for (unsigned int i = 0; i < 1 << bound; i++)
		xn_a[i] = xn.read();
	fft_sim<CONFIG_T>(xn_a, xk_a, &status, &config);
	for (unsigned int i = 0; i < 1 << bound; i++)
		xk.write(xk_a[i]);
	status_data_V.write(status);
#endif
}

// 1-channel, fixed-point, arrays + streams
template<
typename CONFIG_T
>
void fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V)
{
#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
#else
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read(config);
	fft_sim<CONFIG_T>(xn, xk, &status, &config);
	status_data_V.write(status);
#endif
}

// 1-channel, fixed-point, arrays + streams, non blocking for simulation
template<
typename CONFIG_T, fft_T2_t tag
>
void fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V)
{
#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
#else
	ip_fft::status_t<CONFIG_T> status;
	static ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read_nb(config);
	fft_sim<CONFIG_T>(xn, xk, &status, &config);
	status_data_V.write(status);
#endif
}


// 1-channel, fixed-point, arrays + scalars, inlined
template<
typename CONFIG_T, fft_T0_t tag
>
void fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch)
{
#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn 
#pragma HLS stream variable=xk 
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	config_ch_data_V.write(*config_ch);
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
	*status = status_data_V.read();
#else
	fft_sim<CONFIG_T>(xn, xk, status, config_ch);
#endif
}


// 1-channel, fixed-point, arrays + scalars, not inlined
template<
typename CONFIG_T
>
void fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch)
{
#ifdef __SYNTHESIS__
#pragma HLS dataflow
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	config_ch_data_V.write(*config_ch);
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
	*status = status_data_V.read();
#else
	fft_sim<CONFIG_T>(xn, xk, status, config_ch);
#endif
}

#ifdef __SYNTHESIS__

template<
typename CONFIG_T
>
void fft_copy(hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V, ip_fft::status_t<CONFIG_T> *status, bool &flag) {

	if (flag)		    *status = status_data_V.read();
}

template<
typename CONFIG_T
>
void fft_wrapper(std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V,
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V, bool &flag) {
#pragma HLS dataflow
        fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
        flag = true;

}

#endif

// 1-channel, fixed-point, arrays + scalars, with extra wrapper 
template<
typename CONFIG_T, fft_T3_t tag
>
void fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch)
{
#ifdef __SYNTHESIS__
#pragma HLS dataflow
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	bool flag;
#pragma HLS STREAM variable=flag type=pipo
	config_ch_data_V.write(*config_ch);
	fft_wrapper<CONFIG_T>(xn, xk, config_ch_data_V, status_data_V, flag); // not inlined
	fft_copy(status_data_V, status, flag);
#else
	fft_sim<CONFIG_T>(xn, xk, status, config_ch);
#endif
}

// 1-channel, fixed-point, 1D arrays + scalar-to-stream, not inlined
template<
typename CONFIG_T, fft_T1_t tag
>
void       fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status_data_V,
		ip_fft::config_t<CONFIG_T> *config_ch_data_V)
{

#ifdef __SYNTHESIS__
#pragma HLS inline off
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
#pragma HLS stream variable=status_data_V
#pragma HLS stream variable=config_ch_data_V
	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	1,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate>(xn, xk, status_data_V, config_ch_data_V); // inlined
#else
	fft_sim<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V);
#endif
} // End of 1-channel, fixed-point

// Multi-channels, fixed-point, 2D arrays + streams
template<
typename CONFIG_T
>
void   fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8,
		((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V)
{
#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
#pragma HLS array_reshape dim=1 variable=xn
#pragma HLS array_reshape dim=1 variable=xk	
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
#else
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read(config);

	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	CONFIG_T::channels,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, &status, &config);
	status_data_V.write(status);

#endif
} 

// Multi-channels, fixed-point, 2D arrays + streams, non-blocking for simulation
template<
typename CONFIG_T, fft_T2_t tag
>
void   fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8,
		((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V)
{
#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
#pragma HLS array_reshape dim=1 variable=xn
#pragma HLS array_reshape dim=1 variable=xk	
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
#else
	ip_fft::status_t<CONFIG_T> status;
	static ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read_nb(config);

	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	CONFIG_T::channels,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, &status, &config);
	status_data_V.write(status);

#endif
} 

// Multi-channels, fixed-point, 2D arrays + scalars, not inlined
template<
typename CONFIG_T
>
void   fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8,
		((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T>* status,
		ip_fft::config_t<CONFIG_T>* config_ch)
{
#ifdef __SYNTHESIS__
#pragma HLS dataflow
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
#pragma HLS array_reshape dim=1 variable=xn
#pragma HLS array_reshape dim=1 variable=xk	
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	config_ch_data_V.write(*config_ch);
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
	*status = status_data_V.read();
#else
	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	CONFIG_T::channels,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, status, config_ch);
#endif
} 

// Multi-channels, fixed-point, 2D arrays + scalars, inlined
template<
typename CONFIG_T, fft_T0_t tag
>
void   fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8,
		((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T>* status,
		ip_fft::config_t<CONFIG_T>* config_ch)
{
#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
#pragma HLS array_reshape dim=1 variable=xn
#pragma HLS array_reshape dim=1 variable=xk	
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	config_ch_data_V.write(*config_ch);
	fft_syn<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V); // not inlined
	*status = status_data_V.read();
#else
	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	CONFIG_T::channels,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, status, config_ch);
#endif
}

// Multi-channels, fixed-point, 2D arrays + scalar-to-stream, not inlined 
template<
typename CONFIG_T, fft_T1_t tag
>
void   fft(
		std::complex<ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1> > xn[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		std::complex<ap_fixed<((CONFIG_T::output_width+7)/8)*8,
		((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1> > xk[CONFIG_T::channels][1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T>* status_data_V,
		ip_fft::config_t<CONFIG_T>* config_ch_data_V)
{
#pragma HLS inline off
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
#pragma HLS array_reshape dim=1 variable=xn
#pragma HLS array_reshape dim=1 variable=xk	
#pragma HLS stream variable=status_data_V
#pragma HLS stream variable=config_ch_data_V
        
	fft_core<
	CONFIG_T,
	CONFIG_T::input_width,
	CONFIG_T::output_width,
	ap_fixed<((CONFIG_T::input_width+7)/8)*8, 1>,
	ap_fixed<((CONFIG_T::output_width+7)/8)*8, ((CONFIG_T::output_width+7)/8)*8-CONFIG_T::input_width+1>,
	1 << CONFIG_T::max_nfft,
	CONFIG_T::channels,
	ip_fft::fixed_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, status_data_V, config_ch_data_V); // inlined

} // End of multi-channels, fixed-point


union U {
	float f;
	unsigned i;
};

template <typename CONFIG_T>
static void
data_copy_from_float_to_int64(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		uint64_t xn_cp[1 << CONFIG_T::max_nfft]) {
	for (unsigned i = 0; i < (1 << CONFIG_T::max_nfft); i++) {
		std::complex<float> xn_tmp = xn[i];
		U u;
		u.f = xn_tmp.real();
		uint64_t xn_cp_tmp = u.i;
		u.f = xn_tmp.imag();
		xn_cp_tmp |= (((uint64_t)u.i) << 32);
		xn_cp[i] = xn_cp_tmp;
	}
}

template <typename CONFIG_T>
static void
data_copy_from_int64_to_float(std::complex<float> xk[1 << CONFIG_T::max_nfft],
		uint64_t xk_cp[1 << CONFIG_T::max_nfft]) {
	for (unsigned i = 0; i < (1 << CONFIG_T::max_nfft); i++) {
		uint64_t xk_cp_tmp = xk_cp[i];
		U u;
		u.i = unsigned(xk_cp_tmp);
		std::complex<float> xk_tmp;
		xk_tmp.real(u.f);
		u.i = unsigned(xk_cp_tmp >> 32);
		xk_tmp.imag(u.f);
		xk[i] = xk_tmp;
	}
}

#ifdef __SYNTHESIS__

// 1-channel, floating-point
template <typename CONFIG_T, char FFT_INPUT_WIDTH, char FFT_OUTPUT_WIDTH,
int FFT_LENGTH, char FFT_CHANNELS, ip_fft::type FFT_DATA_FORMAT>
void fft_syn(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#pragma HLS inline off


__fpga_ip("Vivado_FFT",
		//"component_name", "xfft_0",
		"channels", FFT_CHANNELS,
		"transform_length", FFT_LENGTH,
		"implementation_options", CONFIG_T::arch_opt,
		"run_time_configurable_transform_length", CONFIG_T::has_nfft,
		"data_format", ip_fft::fft_data_format_str[FFT_DATA_FORMAT],
		"input_width", FFT_INPUT_WIDTH,
		"output_width", FFT_OUTPUT_WIDTH,
		"phase_factor_width", CONFIG_T::phase_factor_width,
		"scaling_options", CONFIG_T::scaling_opt,
		"rounding_modes", CONFIG_T::rounding_opt,
		"aclken", "true",
		"aresetn", "true",
		"ovflo", CONFIG_T::ovflo,
		"xk_index", CONFIG_T::xk_index,
		"throttle_scheme", "nonrealtime",
		"output_ordering", CONFIG_T::ordering_opt,
		"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
		"memory_options_data", CONFIG_T::mem_data,
		"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
		"memory_options_reorder", CONFIG_T::mem_reorder,
		"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
		"memory_options_hybrid", CONFIG_T::mem_hybrid,
		"complex_mult_type", CONFIG_T::complex_mult_type,
		"butterfly_type", CONFIG_T::butterfly_type,
		"super_sample_rates", CONFIG_T::super_sample_rate
);

ip_fft::config_t<CONFIG_T> config_tmp = config_ch_data_V.read();
bool has_scaling_sch = config_tmp.getSch();
bool has_direction = config_tmp.getDir();

if (has_direction || has_scaling_sch)
	for (int i = 0; i < FFT_LENGTH; ++i) {
		xk[i] = xn[i];
	}

ip_fft::status_t<CONFIG_T> status_tmp;
status_tmp.data = config_tmp.getDir();
status_data_V.write(status_tmp);
}


template <typename CONFIG_T, char FFT_INPUT_WIDTH, char FFT_OUTPUT_WIDTH,
int FFT_LENGTH, char FFT_CHANNELS, ip_fft::type FFT_DATA_FORMAT>
void fft_syn(hls::stream<std::complex<float>> &xn,
		hls::stream<std::complex<float>> &xk,
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#pragma HLS inline

__fpga_ip("Vivado_FFT",
		//"component_name", "xfft_0",
		"channels", FFT_CHANNELS,
		"transform_length", FFT_LENGTH,
		"implementation_options", CONFIG_T::arch_opt,
		"run_time_configurable_transform_length", CONFIG_T::has_nfft,
		"data_format", ip_fft::fft_data_format_str[FFT_DATA_FORMAT],
		"input_width", FFT_INPUT_WIDTH,
		"output_width", FFT_OUTPUT_WIDTH,
		"phase_factor_width", CONFIG_T::phase_factor_width,
		"scaling_options", CONFIG_T::scaling_opt,
		"rounding_modes", CONFIG_T::rounding_opt,
		"aclken", "true",
		"aresetn", "true",
		"ovflo", CONFIG_T::ovflo,
		"xk_index", CONFIG_T::xk_index,
		"throttle_scheme", "nonrealtime",
		"output_ordering", CONFIG_T::ordering_opt,
		"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
		"memory_options_data", CONFIG_T::mem_data,
		"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
		"memory_options_reorder", CONFIG_T::mem_reorder,
		"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
		"memory_options_hybrid", CONFIG_T::mem_hybrid,
		"complex_mult_type", CONFIG_T::complex_mult_type,
		"butterfly_type", CONFIG_T::butterfly_type,
		"super_sample_rates", CONFIG_T::super_sample_rate 
);

ip_fft::config_t<CONFIG_T> config_tmp = config_ch_data_V.read();
bool has_scaling_sch = config_tmp.getSch();
bool has_direction = config_tmp.getDir();

if (has_direction || has_scaling_sch)
	for (int i = 0; i < FFT_LENGTH; ++i) {
		xk.write(xn.read());
	}

ip_fft::status_t<CONFIG_T> status_tmp;
status_tmp.data = config_tmp.getDir();
status_data_V.write(status_tmp);
}


template <typename CONFIG_T>
void fft_syn(hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> &xn,
		hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> &xk,
		//hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#pragma HLS inline off

__fpga_ip("Vivado_FFT",
		//"component_name", "xfft_0",
		"channels", 1,
		"transform_length", 1 << CONFIG_T::max_nfft,
		"implementation_options", CONFIG_T::arch_opt,
		"run_time_configurable_transform_length", CONFIG_T::has_nfft,
		"data_format", ip_fft::fft_data_format_str[ip_fft::native_floating_point],
		"input_width", 32,
		"output_width", 32,
		"phase_factor_width", CONFIG_T::phase_factor_width,
		"scaling_options", CONFIG_T::scaling_opt,
		"rounding_modes", CONFIG_T::rounding_opt,
		"aclken", "true",
		"aresetn", "true",
		"ovflo", CONFIG_T::ovflo,
		"xk_index", CONFIG_T::xk_index,
		"throttle_scheme", "nonrealtime",
		"output_ordering", CONFIG_T::ordering_opt,
		"cyclic_prefix_insertion", CONFIG_T::cyclic_prefix_insertion,
		"memory_options_data", CONFIG_T::mem_data,
		"memory_options_phase_factors", CONFIG_T::mem_phase_factors,
		"memory_options_reorder", CONFIG_T::mem_reorder,
		"number_of_stages_using_block_ram_for_data_and_phase_factors", CONFIG_T::stages_block_ram,
		"memory_options_hybrid", CONFIG_T::mem_hybrid,
		"complex_mult_type", CONFIG_T::complex_mult_type,
		"butterfly_type", CONFIG_T::butterfly_type,
		"super_sample_rates", CONFIG_T::super_sample_rate 
);

ip_fft::config_t<CONFIG_T> config_tmp = config_ch_data_V.read();
bool has_scaling_sch = config_tmp.getSch();
bool has_direction = config_tmp.getDir();

if (has_direction || has_scaling_sch)
	for (int i = 0; i < (1 << CONFIG_T::max_nfft ); ++i) {
		xk.write(xn.read());
	}

//ip_fft::status_t<CONFIG_T> status_tmp;
//status_tmp.data = config_tmp.getDir();
//status_data_V.write(status_tmp);
}


template<
typename CONFIG_T
>
void fft_wrapper(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::config_t<CONFIG_T> > &config_ch_data_V,
		hls::stream<ip_fft::status_t<CONFIG_T> > &status_data_V, bool &flag) {
#pragma HLS dataflow
        fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,
			ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // not inlined
        flag = true;
}

#endif

// 1-channel, floating-point
template <typename CONFIG_T>
void fft_sim(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch) {
	fft_core<
	CONFIG_T,
	32,
	32,
	float,
	float,
	1 << CONFIG_T::max_nfft,
	1,
	ip_fft::floating_point,
	CONFIG_T::super_sample_rate
	>(xn, xk, status, config_ch);
} 

// 1-channel, floating-point, streaming
template <typename CONFIG_T>
void fft(hls::stream<std::complex<float>> &xn,
		hls::stream<std::complex<float>> &xk,
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#pragma HLS inline off
#ifdef __SYNTHESIS__
	fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,
			ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // inlined
#else
	std::complex<float> xn_a[1 << CONFIG_T::max_nfft];
	std::complex<float> xk_a[1 << CONFIG_T::max_nfft];
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config = config_ch_data_V.read();
	unsigned int bound = CONFIG_T::has_nfft ? config.getNfft() : CONFIG_T::max_nfft;
	for (unsigned int i = 0; i < 1 << bound; i++)
		xn_a[i] = xn.read();
	fft_sim<CONFIG_T>(xn_a, xk_a, &status, &config);
	for (unsigned int i = 0; i < 1 << bound; i++)
		xk.write(xk_a[i]);
	status_data_V.write(status);
#endif
} 

// 1-channel, floating-point, streaming, non-blocking for simulation
template <typename CONFIG_T, fft_T2_t tag>
void fft(hls::stream<std::complex<float>> &xn,
		hls::stream<std::complex<float>> &xk,
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#pragma HLS inline off
#ifdef __SYNTHESIS__
	fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,
			ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // inlined
#else
	std::complex<float> xn_a[1 << CONFIG_T::max_nfft];
	std::complex<float> xk_a[1 << CONFIG_T::max_nfft];
	ip_fft::status_t<CONFIG_T> status;
	static ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read_nb(config);
	unsigned int bound = CONFIG_T::has_nfft ? config.getNfft() : CONFIG_T::max_nfft;
	for (unsigned int i = 0; i < 1 << bound; i++)
		xn_a[i] = xn.read();
	fft_sim<CONFIG_T>(xn_a, xk_a, &status, &config);
	for (unsigned int i = 0; i < 1 << bound; i++)
		xk.write(xk_a[i]);
	status_data_V.write(status);
#endif
} 


template<typename CONFIG_T>
void fft_stream_assemble(hls::stream<std::complex<float>> (&xn_in)[CONFIG_T::super_sample_rate],
		hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> &xn) {
#pragma HLS inline off
	for(int i = 0; i < (1 << (CONFIG_T::max_nfft)) / CONFIG_T::super_sample_rate; i++){
		#pragma HLS PIPELINE II=1
		ip_fft::data_item<CONFIG_T::super_sample_rate> din;
		for(int j = 0; j < CONFIG_T::super_sample_rate; j++){
			#pragma HLS UNROLL
			din.data[j] = xn_in[j].read();
		}
		xn.write(din);
	}
}


template<typename CONFIG_T>
void fft_stream_dissemble(hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> &xk,
		hls::stream<std::complex<float>> (&xk_out)[CONFIG_T::super_sample_rate]) {
#pragma HLS inline off
	for(int i = 0; i < (1 << (CONFIG_T::max_nfft)) / CONFIG_T::super_sample_rate; i++){
		#pragma HLS PIPELINE II=1
		ip_fft::data_item<CONFIG_T::super_sample_rate> dout = xk.read();
		for(int j = 0; j < CONFIG_T::super_sample_rate; j++){
			#pragma HLS UNROLL
			xk_out[j].write(dout.data[j]);
		}
	}
}


// 1-channe, native floating point with ssr > 1
template<typename CONFIG_T>
void fft(hls::stream<std::complex<float>> (&xn_in)[CONFIG_T::super_sample_rate],
		hls::stream<std::complex<float>> (&xk_out)[CONFIG_T::super_sample_rate],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {

#ifdef __SYNTHESIS__
#pragma HLS inline off
#pragma HLS DATAFLOW

	hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> xn;
	hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> xk;

	fft_stream_assemble<CONFIG_T>(xn_in, xn);

	fft_syn<CONFIG_T>(xn,xk,config_ch_data_V);
	
	fft_stream_dissemble<CONFIG_T>(xk, xk_out);

	ip_fft::status_t<CONFIG_T> status_tmp;
	status_tmp.data = 0;
	status_data_V.write(status_tmp);
#else

	std::complex<float> xn_a[1 << CONFIG_T::max_nfft];
	std::complex<float> xk_a[1 << CONFIG_T::max_nfft];
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config = config_ch_data_V.read();
	unsigned int bound = CONFIG_T::has_nfft ? config.getNfft() : CONFIG_T::max_nfft;	
	for (unsigned int i = 0; i < 1 << bound; i++){
		xn_a[i] = xn_in[i % CONFIG_T::super_sample_rate].read();
	}
	fft_sim<CONFIG_T>(xn_a, xk_a, &status, &config);
	for (unsigned int i = 0; i < 1 << bound; i++)
		xk_out[i % CONFIG_T::super_sample_rate].write(xk_a[i]);
	status_data_V.write(status);
#endif
}


template<typename CONFIG_T>
void fft_array_assemble(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> &xn_buf) {
#pragma HLS inline off
	for(int i = 0; i < (1 << (CONFIG_T::max_nfft)) / CONFIG_T::super_sample_rate; i++){
		#pragma HLS PIPELINE II=1
		ip_fft::data_item<CONFIG_T::super_sample_rate> din;
		for(int j = 0; j < CONFIG_T::super_sample_rate; j++){
			#pragma HLS UNROLL
			din.data[j] = xn[i*CONFIG_T::super_sample_rate + j];
		}
		xn_buf.write(din);
	}
}


template<typename CONFIG_T>
void fft_array_dissemble(hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> &xk_buf,
		std::complex<float> xk[1 << CONFIG_T::max_nfft]) {
#pragma HLS inline off
	for(int i = 0; i < (1 << (CONFIG_T::max_nfft)) / CONFIG_T::super_sample_rate; i++){
		#pragma HLS PIPELINE II=1
		ip_fft::data_item<CONFIG_T::super_sample_rate> dout = xk_buf.read();
		for(int j = 0; j < CONFIG_T::super_sample_rate; j++){
			#pragma HLS UNROLL
			xk[i*CONFIG_T::super_sample_rate + j] = dout.data[j];
		}
	}	
}


// 1-channel, floating-point, 1D arrays + streams
template <typename CONFIG_T, std::enable_if_t<CONFIG_T::super_sample_rate == 1, int> = 0>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#ifdef __SYNTHESIS__
	#pragma HLS inline
	#pragma HLS aggregate variable=xn
	#pragma HLS aggregate variable=xk
	#pragma HLS stream variable=xn
	#pragma HLS stream variable=xk
		fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // not inlined
#else
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config = config_ch_data_V.read();
	fft_sim<CONFIG_T>(xn, xk, &status, &config);
	status_data_V.write(status);
#endif
} 


template <typename CONFIG_T, std::enable_if_t<CONFIG_T::super_sample_rate != 1, int> = 0>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {
#ifdef __SYNTHESIS__

	#pragma HLS inline off
	#pragma HLS DATAFLOW

	hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> xn_buf;
	hls::stream<ip_fft::data_item<CONFIG_T::super_sample_rate>> xk_buf;

	fft_array_assemble<CONFIG_T>(xn, xn_buf);

	fft_syn<CONFIG_T>(xn_buf,xk_buf,config_ch_data_V);
	
	fft_array_dissemble<CONFIG_T>(xk_buf, xk);

	ip_fft::status_t<CONFIG_T> status_tmp;
	status_tmp.data = 0;
	status_data_V.write(status_tmp);	
#else
	ip_fft::status_t<CONFIG_T> status;
	ip_fft::config_t<CONFIG_T> config;

	config = config_ch_data_V.read();
	fft_sim<CONFIG_T>(xn, xk, &status, &config);
	status_data_V.write(status);
#endif
} 


// 1-channel, floating-point, 1D arrays + streams, non-blocking for simulation
template <typename CONFIG_T, fft_T2_t tag>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		hls::stream<ip_fft::status_t<CONFIG_T>> &status_data_V,
		hls::stream<ip_fft::config_t<CONFIG_T>> &config_ch_data_V) {

#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,
			ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // not inlined
#else
	ip_fft::status_t<CONFIG_T> status;
	static ip_fft::config_t<CONFIG_T> config;

	config_ch_data_V.read_nb(config);
	fft_sim<CONFIG_T>(xn, xk, &status, &config);
	status_data_V.write(status);
#endif
} 


// 1-channel, floating-point, 1D arrays + scalars, not inlined
template <typename CONFIG_T>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch) {

#ifdef __SYNTHESIS__
#pragma HLS dataflow
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	config_ch_data_V.write(*config_ch);
	fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,
			ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // not inlined
	*status = status_data_V.read();

#else
	fft_sim<CONFIG_T>(xn, xk, status, config_ch);
#endif
} 

// 1-channel, floating-point, 1D arrays + scalars, inlined
template <typename CONFIG_T, fft_T0_t tag>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch) {

#ifdef __SYNTHESIS__
#pragma HLS inline
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk
	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
	config_ch_data_V.write(*config_ch);
	fft_syn<CONFIG_T, 32, 32, 1 << CONFIG_T::max_nfft, 1,
			ip_fft::floating_point>(xn, xk, status_data_V, config_ch_data_V); // not inlined
	*status = status_data_V.read();

#else
	fft_sim<CONFIG_T>(xn, xk, status, config_ch);
#endif
} 

// 1-channel, floating-point, 1D arrays + scalar-to-stream, not inlined
template <typename CONFIG_T, fft_T1_t tag>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status_data_V,
		ip_fft::config_t<CONFIG_T> *config_ch_data_V) {

#ifdef __SYNTHESIS__
#pragma HLS inline off
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk

#pragma HLS stream variable=status_data_V
#pragma HLS stream variable=config_ch_data_V

	fft_core<CONFIG_T, 32, 32, float, float, 1 << CONFIG_T::max_nfft, 1, ip_fft::floating_point, CONFIG_T::super_sample_rate>(xn, xk, status_data_V, config_ch_data_V); // inlined
#else
	fft_sim<CONFIG_T>(xn, xk, status_data_V, config_ch_data_V);
#endif
} 

// 1-channel, floating-point, arrays + scalars, with extra wrapper 
template <typename CONFIG_T, fft_T3_t tag>
void fft(std::complex<float> xn[1 << CONFIG_T::max_nfft],
		std::complex<float> xk[1 << CONFIG_T::max_nfft],
		ip_fft::status_t<CONFIG_T> *status,
		ip_fft::config_t<CONFIG_T> *config_ch) {

#ifdef __SYNTHESIS__
#pragma HLS dataflow
#pragma HLS aggregate variable=xn
#pragma HLS aggregate variable=xk
#pragma HLS stream variable=xn
#pragma HLS stream variable=xk

	hls::stream<ip_fft::config_t<CONFIG_T>, 2> config_ch_data_V;
	hls::stream<ip_fft::status_t<CONFIG_T>, 2> status_data_V;
        bool flag;
#pragma HLS STREAM variable=flag type=pipo
	config_ch_data_V.write(*config_ch);
	fft_wrapper<CONFIG_T>(xn, xk, config_ch_data_V, status_data_V, flag); // not inlined
        fft_copy(status_data_V, status, flag);
#else
	fft_sim<CONFIG_T>(xn, xk, status, config_ch);
#endif
} // End of 1-channel, floating-point 


namespace ip_fft {

template<typename config_t>
void get_status(hls::stream<hls::ip_fft::status_t<config_t>> &status_strm, 
                bool &ovflo) {
    ovflo = status_strm.read().getOvflo() & 0x1;
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_out_t>
void get_data(unsigned log2_of_length, 
              std::complex<data_out_t>  in[1 << log2_of_max_length], 
              std::complex<data_out_t>  out[1 << log2_of_max_length]) {
        unsigned length = 1 << log2_of_length;
        assert(log2_of_length <= log2_of_max_length);
        assert(length <= 1 << log2_of_max_length);
        assert(length>0);
        for (int i=0; i<length; i++) {
#pragma HLS pipeline II=1 rewind style=flp
                std::complex<data_out_t> tmp = in[i];
                out[i] = tmp;
        }
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_out_t>
void get_data(unsigned log2_of_length, 
              std::complex<data_out_t>  in[1 << log2_of_max_length], 
              hls::stream<std::complex<data_out_t>>&  out) {
        unsigned length = 1 << log2_of_length;
        assert(log2_of_length <= log2_of_max_length);
        assert(length <= 1 << log2_of_max_length);
        assert(length>0);
        for (int i=0; i<length; i++) {
#pragma HLS pipeline II=1 rewind style=flp
                std::complex<data_out_t> tmp = in[i];
                out.write(tmp);
        }
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_out_t>
void outputdatamover(unsigned int log2_of_length,
                     std::complex<data_out_t>  in[1 << log2_of_max_length], 
                     hls::stream<std::complex<data_out_t>>&  out,
                     hls::stream<hls::ip_fft::status_t<config_t>>  &status_strm,
                     bool& ovflo) {
#pragma HLS dataflow
#pragma HLS inline

        get_status<config_t>(status_strm, ovflo);
        get_data<log2_of_max_length, config_t, data_out_t>(log2_of_length, in, out);
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_out_t>
void outputdatamover(unsigned int log2_of_length,
                     std::complex<data_out_t>  in[1 << log2_of_max_length], 
                     std::complex<data_out_t>  out[1 << log2_of_max_length],
                     hls::stream<hls::ip_fft::status_t<config_t>>  &status_strm,
                     bool& ovflo) {
#pragma HLS dataflow
#pragma HLS inline

        get_status<config_t>(status_strm, ovflo);
        get_data<log2_of_max_length, config_t, data_out_t>(log2_of_length, in, out);
}

template<typename config_t>
void set_config(bool direction, 
                unsigned scaling_sch,
                unsigned cyclic_prefix_length,
                hls::ip_fft::config_t<config_t> &config) {
        hls::ip_fft::config_t<config_t> config_tmp;
        config_tmp.setDir(direction);
        config_tmp.setSch(scaling_sch);
        //config_tmp.setCpLen(cyclic_prefix_length);
        config = config_tmp;
}

template<typename config_t>
void set_config(bool direction, 
                unsigned int log2_of_max_length,
                unsigned scaling_sch,
                unsigned cyclic_prefix_length,
                hls::ip_fft::config_t<config_t> &config) {
        hls::ip_fft::config_t<config_t> config_tmp;
        config_tmp.setDir(direction);
        config_tmp.setSch(scaling_sch);
        //config_tmp.setCpLen(cyclic_prefix_length);
        config_tmp.setNfft(log2_of_max_length);
        config = config_tmp;
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_in_t>
void set_data(unsigned int log2_of_length,
              hls::stream<std::complex<data_in_t>>& in,
              std::complex<data_in_t>  out[1 << log2_of_max_length]) {
        assert(log2_of_length <= log2_of_max_length);
        unsigned length = 1 << log2_of_length;
        assert(length <= 1 << log2_of_max_length);
        assert(length>0);
        
        for (int i=0; i<length; i++) {
#pragma HLS pipeline II=1 rewind style=flp
            out[i] = in.read();
        }
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_in_t>
void inputdatamover(hls::stream<std::complex<data_in_t>>&  in,
                    std::complex<data_in_t>  out[1 << log2_of_max_length],
                    bool direction,
                    unsigned scaling_sch,
                    unsigned cyclic_prefix_length,
                    hls::ip_fft::config_t<config_t> &config) {
#pragma HLS dataflow
#pragma HLS inline

    set_config<config_t>(direction, scaling_sch, cyclic_prefix_length, config);
	set_data<log2_of_max_length, config_t, data_in_t>(log2_of_max_length, in, out);
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_in_t>
void inputdatamover(unsigned int log2_of_length,
                    hls::stream<std::complex<data_in_t>>&  in,
                    std::complex<data_in_t>  out[1 << log2_of_max_length],
                    bool direction,
                    unsigned scaling_sch,
                    unsigned cyclic_prefix_length,
                    hls::ip_fft::config_t<config_t> &config) {
#pragma HLS dataflow
#pragma HLS inline

        set_config<config_t>(direction, log2_of_length, scaling_sch, cyclic_prefix_length, config);
        set_data<log2_of_max_length, config_t, data_in_t>(log2_of_length, in, out);
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_in_t>
void set_data(unsigned int log2_of_length,
              std::complex<data_in_t>  in[1 << log2_of_max_length], 
              std::complex<data_in_t>  out[1 << log2_of_max_length]) {
        assert(log2_of_length <= log2_of_max_length);
        unsigned length = 1 << log2_of_length;
        assert(length <= 1 << log2_of_max_length);
        assert(length>0);
        
        for (int i=0; i<length; i++) {
#pragma HLS pipeline II=1 rewind style=flp
            out[i] = in[i];
        }
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_in_t>
void inputdatamover(std::complex<data_in_t>  in[1 << log2_of_max_length], 
                    std::complex<data_in_t>  out[1 << log2_of_max_length],
                    bool direction,
                    unsigned scaling_sch,
                    unsigned cyclic_prefix_length,
                    hls::ip_fft::config_t<config_t> &config) {
#pragma HLS dataflow
#pragma HLS inline

    set_config<config_t>(direction, scaling_sch, cyclic_prefix_length, config);
	set_data<log2_of_max_length, config_t, data_in_t>(log2_of_max_length, in, out);
}

template<unsigned int log2_of_max_length,
         typename config_t,
         typename data_in_t>
void inputdatamover(unsigned int log2_of_length,
                    std::complex<data_in_t>  in[1 << log2_of_max_length], 
                    std::complex<data_in_t>  out[1 << log2_of_max_length],
                    bool direction,
                    unsigned scaling_sch,
                    unsigned cyclic_prefix_length,
                    hls::ip_fft::config_t<config_t> &config) {
#pragma HLS dataflow
#pragma HLS inline

        set_config<config_t>(direction, log2_of_length, scaling_sch, cyclic_prefix_length, config);
        set_data<log2_of_max_length, config_t, data_in_t>(log2_of_length, in, out);
}

template<unsigned int log2_of_max_length,
         typename config_t, 
         typename data_in_t,
         typename data_out_t>
void fft_call(std::complex<data_in_t>  xn[1 << log2_of_max_length], 
              std::complex<data_out_t> xk[1 << log2_of_max_length], 
              hls::ip_fft::config_t<config_t> config, 
              hls::stream<hls::ip_fft::status_t<config_t>> &status_strm) {
#pragma HLS DATAFLOW

    hls::stream<hls::ip_fft::config_t<config_t>> config_strm;

    config_strm.write(config);
    hls::fft<config_t>(xn, xk, status_strm, config_strm);
}




template<unsigned int log2_of_max_length, 
         enum hls::ip_fft::arch implementation_options,
         bool run_time_configurable_transform_length, 
         unsigned int in_width,
         unsigned int out_width, 
         unsigned int ph_factor_width,
         unsigned int scaling_options,
         enum hls::ip_fft::rounding rounding_modes, 
         enum hls::ip_fft::ordering output_ordering,
         bool cyclic_prefix_ins,
         enum hls::ip_fft::mem memory_options_data,
         enum hls::ip_fft::mem memory_options_phase_factors,
         enum hls::ip_fft::mem memory_options_reorder,
         bool memory_options_hybrid,
         enum hls::ip_fft::opt cplx_mult_type,
         enum hls::ip_fft::opt butterfly_impl,
         enum hls::ip_fft::type data_type = hls::ip_fft::fixed_point>
struct fft_config_t : hls::ip_fft::params_t {
    static const unsigned input_width = in_width;
    static const unsigned output_width = out_width;
    static const unsigned status_width = 8;
    static const unsigned max_nfft = log2_of_max_length;
    static const bool has_nfft = run_time_configurable_transform_length;
    static const unsigned channels = 1;
    static const unsigned arch_opt = implementation_options;
    static const unsigned phase_factor_width = ph_factor_width;
    static const unsigned ordering_opt = output_ordering;
    static const bool ovflo = true;
    static const unsigned scaling_opt = scaling_options;
    static const unsigned rounding_opt = rounding_modes;
    static const unsigned mem_data = memory_options_data;
    static const unsigned mem_phase_factors = memory_options_phase_factors;
    static const unsigned mem_reorder = memory_options_reorder;
    static const unsigned stages_block_ram = (log2_of_max_length < 10) ? 0 : (log2_of_max_length - 9);
    static const bool mem_hybrid = memory_options_hybrid;
    static const unsigned complex_mult_type = cplx_mult_type;
    static const unsigned butterfly_type = butterfly_impl;
    static const bool xk_index = false;
    static const unsigned nfft_bits = has_nfft ? 8 : 0; // Padding to 8 bits
    static const unsigned cp_len_bits = cyclic_prefix_ins ? (((max_nfft + 7) >> 3) << 3) : 0; // Padding
    static const unsigned tmp_bits = (arch_opt == unsigned(ip_fft::pipelined_streaming_io) || arch_opt == unsigned(ip_fft::radix_4_burst_io)) ? ((max_nfft+1)>>1) * 2 : 2 * max_nfft;
    static const bool need_scaling = (data_type == ip_fft::floating_point) ? true : (scaling_opt == unsigned(ip_fft::scaled));
    static const unsigned sch_bits = need_scaling ? tmp_bits : 0;
    static const unsigned config_bits = (sch_bits + channels) * channels + cp_len_bits + nfft_bits;
    static const unsigned config_width = ((config_bits + 7) >> 3) << 3; // padding
};

}; // namespace ip_fft

} // namespace hls

namespace xf {
namespace dsp {

template<unsigned int log2_of_max_length = 10, 
         unsigned int input_width = 16,
         unsigned int output_width = 16,
         unsigned int phase_factor_width = 16,
         enum hls::ip_fft::arch implementation_options = hls::ip_fft::pipelined_streaming_io,
         enum hls::ip_fft::scaling scaling_options = hls::ip_fft::scaled,
         enum hls::ip_fft::rounding rounding_modes = hls::ip_fft::truncation, 
         enum hls::ip_fft::ordering output_ordering = hls::ip_fft::bit_reversed_order,
         bool cyclic_prefix_insertion = false,
         enum hls::ip_fft::mem memory_options_data = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_phase_factors = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_reorder = hls::ip_fft::block_ram,
         bool memory_options_hybrid = false,
         enum hls::ip_fft::opt complex_mult_type = hls::ip_fft::use_mults_resources,
         enum hls::ip_fft::opt butterfly_type = hls::ip_fft::use_luts,
         typename data_in_t,
         typename data_out_t>
void vivado_var_len_fft(
    bool direction,
    unsigned int log2_of_length,
    hls::stream<std::complex<data_in_t>>&in,
    hls::stream<std::complex<data_out_t>>&out,
    bool& ovflo,
    unsigned scaling_sch = 0,
    unsigned cyclic_prefix_length = 0)
{
#pragma HLS dataflow

    typedef hls::ip_fft::fft_config_t<log2_of_max_length, 
         implementation_options, 
         true, // run_time_configurable_transform_length
         input_width, 
         output_width, 
         phase_factor_width,
         scaling_options,
         rounding_modes, 
         output_ordering,
         cyclic_prefix_insertion,
         memory_options_data,
         memory_options_phase_factors,
         memory_options_reorder,
         memory_options_hybrid,
         complex_mult_type,
         butterfly_type> fft_config_t;

    std::complex<data_in_t> xn[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xn depth=8

    std::complex<data_out_t> xk[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xk depth=8

    hls::ip_fft::config_t<fft_config_t>  config;
#pragma HLS STREAM variable=config type=pipo

    hls::stream<hls::ip_fft::status_t<fft_config_t>> status_strm;
#pragma HLS STREAM variable=status_strm depth=8
   
    hls::ip_fft::inputdatamover<log2_of_max_length, fft_config_t, data_in_t>(log2_of_length, in, xn, direction, scaling_sch, cyclic_prefix_length, config);
    hls::ip_fft::fft_call<log2_of_max_length, fft_config_t, data_in_t, data_out_t>(xn, xk, config, status_strm);
    hls::ip_fft::outputdatamover<log2_of_max_length, fft_config_t, data_out_t>(log2_of_length, xk, out, status_strm, ovflo);
}

template<unsigned int log2_of_max_length = 10, 
         unsigned int input_width = 16,
         unsigned int output_width = 16,
         unsigned int phase_factor_width = 16,
         enum hls::ip_fft::arch implementation_options = hls::ip_fft::pipelined_streaming_io,
         enum hls::ip_fft::scaling scaling_options = hls::ip_fft::scaled,
         enum hls::ip_fft::rounding rounding_modes = hls::ip_fft::truncation, 
         enum hls::ip_fft::ordering output_ordering = hls::ip_fft::bit_reversed_order,
         bool cyclic_prefix_insertion = false,
         enum hls::ip_fft::mem memory_options_data = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_phase_factors = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_reorder = hls::ip_fft::block_ram,
         bool memory_options_hybrid = false,
         enum hls::ip_fft::opt complex_mult_type = hls::ip_fft::use_mults_resources,
         enum hls::ip_fft::opt butterfly_type = hls::ip_fft::use_luts,
         typename data_in_t,
         typename data_out_t>
void vivado_fix_len_fft(
    bool direction,
    hls::stream<std::complex<data_in_t>>& in,
    hls::stream<std::complex<data_out_t>>& out,
    bool& ovflo,
    unsigned scaling_sch = 0,
    unsigned cyclic_prefix_length = 0)
{
#pragma HLS dataflow

    typedef hls::ip_fft::fft_config_t<log2_of_max_length, 
         implementation_options, 
         false, // run_time_configurable_transform_length
         input_width, 
         output_width, 
         phase_factor_width,
         scaling_options,
         rounding_modes, 
         output_ordering,
         cyclic_prefix_insertion,
         memory_options_data,
         memory_options_phase_factors,
         memory_options_reorder,
         memory_options_hybrid,
         complex_mult_type,
         butterfly_type> fft_config_t;

    std::complex<data_in_t> xn[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xn depth=8

    std::complex<data_out_t> xk[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xk depth=8

    hls::ip_fft::config_t<fft_config_t>  config;
#pragma HLS STREAM variable=config type=pipo

    hls::stream<hls::ip_fft::status_t<fft_config_t>> status_strm;
   
    hls::ip_fft::inputdatamover<log2_of_max_length, fft_config_t, data_in_t>(in, xn, direction, scaling_sch, cyclic_prefix_length, config);
    hls::ip_fft::fft_call<log2_of_max_length, fft_config_t, data_in_t, data_out_t>(xn, xk, config, status_strm);
    hls::ip_fft::outputdatamover<log2_of_max_length, fft_config_t, data_out_t>(log2_of_max_length, xk, out, status_strm, ovflo);
}

template<unsigned int log2_of_max_length = 10, 
         unsigned int input_width = 16,
         unsigned int output_width = 16,
         unsigned int phase_factor_width = 16,
         enum hls::ip_fft::arch implementation_options = hls::ip_fft::pipelined_streaming_io,
         enum hls::ip_fft::scaling scaling_options = hls::ip_fft::scaled,
         enum hls::ip_fft::rounding rounding_modes = hls::ip_fft::truncation, 
         enum hls::ip_fft::ordering output_ordering = hls::ip_fft::bit_reversed_order,
         bool cyclic_prefix_insertion = false,
         enum hls::ip_fft::mem memory_options_data = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_phase_factors = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_reorder = hls::ip_fft::block_ram,
         bool memory_options_hybrid = false,
         enum hls::ip_fft::opt complex_mult_type = hls::ip_fft::use_mults_resources,
         enum hls::ip_fft::opt butterfly_type = hls::ip_fft::use_luts,
         typename data_in_t,
         typename data_out_t>
void vivado_var_len_fft(
    bool direction,
    unsigned int log2_of_length,
    std::complex<data_in_t> in[1 << log2_of_max_length],
    std::complex<data_out_t> out[1 << log2_of_max_length],
    bool& ovflo,
    unsigned scaling_sch = 0,
    unsigned cyclic_prefix_length = 0)
{
#pragma HLS dataflow

    typedef hls::ip_fft::fft_config_t<log2_of_max_length, 
         implementation_options, 
         true, // run_time_configurable_transform_length
         input_width, 
         output_width, 
         phase_factor_width,
         scaling_options,
         rounding_modes, 
         output_ordering,
         cyclic_prefix_insertion,
         memory_options_data,
         memory_options_phase_factors,
         memory_options_reorder,
         memory_options_hybrid,
         complex_mult_type,
         butterfly_type> fft_config_t;

    std::complex<data_in_t> xn[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xn depth=8

    std::complex<data_out_t> xk[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xk depth=8

    hls::ip_fft::config_t<fft_config_t>  config;
#pragma HLS STREAM variable=config type=pipo

    hls::stream<hls::ip_fft::status_t<fft_config_t>> status_strm;
#pragma HLS STREAM variable=status_strm depth=8
   
    hls::ip_fft::inputdatamover<log2_of_max_length, fft_config_t, data_in_t>(log2_of_length, in, xn, direction, scaling_sch, cyclic_prefix_length, config);
    hls::ip_fft::fft_call<log2_of_max_length, fft_config_t, data_in_t, data_out_t>(xn, xk, config, status_strm);
    hls::ip_fft::outputdatamover<log2_of_max_length, fft_config_t, data_out_t>(log2_of_length, xk, out, status_strm, ovflo);
}

template<unsigned int log2_of_max_length = 10, 
         unsigned int input_width = 16,
         unsigned int output_width = 16,
         unsigned int phase_factor_width = 16,
         enum hls::ip_fft::arch implementation_options = hls::ip_fft::pipelined_streaming_io,
         enum hls::ip_fft::scaling scaling_options = hls::ip_fft::scaled,
         enum hls::ip_fft::rounding rounding_modes = hls::ip_fft::truncation, 
         enum hls::ip_fft::ordering output_ordering = hls::ip_fft::bit_reversed_order,
         bool cyclic_prefix_insertion = false,
         enum hls::ip_fft::mem memory_options_data = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_phase_factors = hls::ip_fft::block_ram,
         enum hls::ip_fft::mem memory_options_reorder = hls::ip_fft::block_ram,
         bool memory_options_hybrid = false,
         enum hls::ip_fft::opt complex_mult_type = hls::ip_fft::use_mults_resources,
         enum hls::ip_fft::opt butterfly_type = hls::ip_fft::use_luts,
         typename data_in_t,
         typename data_out_t>
void vivado_fix_len_fft(
    bool direction,
    std::complex<data_in_t> in[1 << log2_of_max_length],
    std::complex<data_out_t> out[1 << log2_of_max_length],
    bool& ovflo,
    unsigned scaling_sch = 0,
    unsigned cyclic_prefix_length = 0)
{
#pragma HLS dataflow

    typedef hls::ip_fft::fft_config_t<log2_of_max_length, 
         implementation_options, 
         false, // run_time_configurable_transform_length
         input_width, 
         output_width, 
         phase_factor_width,
         scaling_options,
         rounding_modes, 
         output_ordering,
         cyclic_prefix_insertion,
         memory_options_data,
         memory_options_phase_factors,
         memory_options_reorder,
         memory_options_hybrid,
         complex_mult_type,
         butterfly_type> fft_config_t;

    std::complex<data_in_t> xn[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xn depth=8

    std::complex<data_out_t> xk[1 << log2_of_max_length] __attribute__((no_ctor));
#pragma HLS STREAM variable=xk depth=8

    hls::ip_fft::config_t<fft_config_t>  config;
#pragma HLS STREAM variable=config type=pipo

    hls::stream<hls::ip_fft::status_t<fft_config_t>> status_strm;
   
    hls::ip_fft::inputdatamover<log2_of_max_length, fft_config_t, data_in_t>(in, xn, direction, scaling_sch, cyclic_prefix_length, config);
    hls::ip_fft::fft_call<log2_of_max_length, fft_config_t, data_in_t, data_out_t>(xn, xk, config, status_strm);
    hls::ip_fft::outputdatamover<log2_of_max_length, fft_config_t, data_out_t>(log2_of_max_length, xk, out, status_strm, ovflo);
}

} // namespace dsp
} // namespace xf
#endif // __cplusplus
#endif // X_HLS_FFT_H


