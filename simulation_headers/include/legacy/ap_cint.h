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


#ifndef _AUTOPILOT_CINT_H_
#define _AUTOPILOT_CINT_H_

#ifdef __cplusplus
#warning This header file cannot be used in a C++ design. C-synthesis will fail. 
#endif

#if (defined __llvm__) || \
    (defined AESL_TB) || (defined AUTOPILOT_BC_SIM) || (__RTL_SIMULATION__)

#ifndef __openclc
#ifndef __SYNTHESIS__
#include <string.h>
#include <stdio.h>
#endif
#endif

#ifdef __CYGWIN__
#  ifdef feof
#    undef feof
#  endif

#  ifdef ferror
#    undef ferror
#  endif
#endif


#include "etc/autopilot_apint.h"

#else
#  ifdef __cplusplus
#  error This header file cannot be used in a C++ design. 
#  else
#  error This header file does not support debugging. Do not select "Debug" in GUI.
#  endif
#endif /* SYN or AUTOCC */
#endif /* #ifndef _AUTOPILOT_CINT_H_ */


