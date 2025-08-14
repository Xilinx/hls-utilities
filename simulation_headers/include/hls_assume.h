// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef X_HLS_ASSUME_SIM_H
#define X_HLS_ASSUME_SIM_H
#include <assert.h>

#ifndef __cplusplus

static void hls_assume(int pred) { assert(pred); }

#else

namespace hls {
static void assume(bool pred) { assert(pred); }
}  // namespace hls

#endif  // __cplusplus
#endif  // X_HLS_ASSUME_SIM_H
