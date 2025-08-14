// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#ifndef __AP_FLOAT_BF16_H__
#define __AP_FLOAT_BF16_H__

#ifndef __AP_FLOAT_H__
#error "This file should not be included directly. Include ap_float.h instead."
#endif

#ifndef __SYNTHESIS__
// Fast bf16 to ap_float_single conversion
template<> template<>
AP_INLINE AP_NODEBUG
ap_float_single::ap_float(const ap_float_bf16& f16) noexcept {
    bits = f16.bits;
    bits <<= 16;
}

// Fast ap_float_single to bf16 conversion
template<> template<>
AP_INLINE AP_NODEBUG
ap_float_bf16::ap_float(const ap_float_single& f32) noexcept {
    bits = f32.bits >> 16;

    // Subnormal
    if (exponent_ref() == exponent_denorm()) {
        mantissa_ref() = 0; // Flush to zero
        return;
    }

    // Infinites and NaNs
    if (exponent_ref() == exponent_infinity()) {
        if (f32.bits(15, 0).or_reduce())
            bits |= mantissa_quiet_mask(); // Make sure NaNs don't become Infinity
        return;
    }

    // Round to nearest, ties to even
    bool guard = f32.bits[16];
    bool round = f32.bits[15];
    bool sticky = f32.bits(14, 0).or_reduce();
    bool round_up = round && (guard || sticky);
    if (round_up) {
        bits++; // This also increases the exponent when necessary
    }
}
#endif

#endif
