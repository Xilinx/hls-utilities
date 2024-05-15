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


#ifndef X_DSP_BUILTINS_H
#define X_DSP_BUILTINS_H
#include <stdint.h>

#ifdef __cplusplus
#include <ap_int.h>
typedef ap_int<1> __dsp1_t;
typedef ap_int<6> __dsp6_t;
typedef ap_int<17> __dsp17_t;
typedef ap_int<18> __dsp18_t;
typedef ap_int<25> __dsp25_t;
typedef ap_int<27> __dsp27_t;
typedef ap_int<30> __dsp30_t;
typedef ap_int<48> __dsp48_t;
#else
#include <legacy/ap_cint.h>
typedef int1 __dsp1_t;
typedef int6 __dsp6_t;
typedef int17 __dsp17_t;
typedef int18 __dsp18_t;
typedef int25 __dsp25_t;
typedef int27 __dsp27_t;
typedef int30 __dsp30_t;
typedef int48 __dsp48_t;
#endif

#include "etc/dsp_dp_builtins.h"

#endif



