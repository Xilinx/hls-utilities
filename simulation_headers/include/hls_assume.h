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
