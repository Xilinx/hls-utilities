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


#ifndef _X_HLS_ALL_ANY_APFIXED_H_
#define _X_HLS_ALL_ANY_APFIXED_H_
#include "ap_fixed.h"
#include "ap_int.h"

///hls_all ap_fixed
template <int W_, int I_>
bool generic_all(ap_fixed<W_,I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<W_;i++)
    	if (x[i]==0)
    		return false;
    return true;
}

///hls_all ap_ufixed
template <int W_, int I_>
bool generic_all(ap_ufixed<W_,I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<W_;i++)
    	if (x[i]==0)
    		return false;
    return true;
}

///hls_all ap_int
template <int I_>
bool generic_all(ap_int<I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<I_;i++)
    	if (x[i]==0)
    		return false;
    return true;
}

///hls_all ap_uint
template <int I_>
bool generic_all(ap_uint<I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<I_;i++)
    	if (x[i]==0)
    		return false;
    return true;
}

///hls_any ap_fixed
template <int W_, int I_>
bool generic_any(ap_fixed<W_,I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<W_;i++)
    	if (x[i]==1)
    		return true;
    return false;
}

///hls_any ap_ufixed
template <int W_, int I_>
bool generic_any(ap_ufixed<W_,I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<W_;i++)
    	if (x[i]==1)
    		return true;
    return false;
}

///hls_any ap_int
template <int I_>
bool generic_any(ap_int<I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<I_;i++)
    	if (x[i]==1)
    		return true;
    return false;
}

///hls_any ap_uint
template <int I_>
bool generic_any(ap_uint<I_> x)
{
#pragma HLS pipeline II=1
    for (int i=0; i<I_;i++)
    	if (x[i]==1)
    		return true;
    return false;
}

#endif



