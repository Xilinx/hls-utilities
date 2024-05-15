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


#ifndef _X_HLS_isequal_isnotequal_APFIXED_H_
#define _X_HLS_isequal_isnotequal_APFIXED_H_
#include "ap_fixed.h"
#include "stdint.h"
//#include "hls_fpo.h"
//#include "hls_half.h"

///isequal  apfixed a & apfixed b
template <int W_, int I_>
bool generic_isequal(ap_fixed<W_,I_> x,ap_fixed<W_,I_> y)
{
#pragma HLS pipeline
    return (x==y);
}

///isnotequal  apfixed a & apfixed b
template <int W_, int I_>
bool generic_isnotequal(ap_fixed<W_,I_> x,ap_fixed<W_,I_> y)
{
#pragma HLS pipeline
    return (!(x==y));
}

///isequal  ap_ufixed a & ap_fixed b
template <int W_, int I_>
bool generic_isequal(ap_ufixed<W_,I_> x,ap_fixed<W_,I_> y)
{
#pragma HLS pipeline
	return (x==y);
}

///isnotequal  ap_ufixed a & ap_fixed b
template <int W_, int I_>
bool generic_isnotequal(ap_ufixed<W_,I_> x,ap_fixed<W_,I_> y)
{
#pragma HLS pipeline
	return (!(x==y));
}

///isequal  ap_fixed a & ap_ufixed b
template <int W_, int I_>
bool generic_isequal(ap_fixed<W_,I_> x,ap_ufixed<W_,I_> y)
{
#pragma HLS pipeline
	return (x==y);
}

///isnotequal  ap_fixed a & ap_ufixed b
template <int W_, int I_>
bool generic_isnotequal(ap_fixed<W_,I_> x,ap_ufixed<W_,I_> y)
{
#pragma HLS pipeline
	return (!(x==y));
}

///isequal  ap_ufixed a & ap_ufixed b
template <int W_, int I_>
bool generic_isequal(ap_ufixed<W_,I_> x,ap_ufixed<W_,I_> y)
{
#pragma HLS pipeline
	return (x==y);
}

///isnotequal  ap_ufixed a & ap_ufixed b
template <int W_, int I_>
bool generic_isnotequal(ap_ufixed<W_,I_> x,ap_ufixed<W_,I_> y)
{
#pragma HLS pipeline
	return (!(x==y));
}


///isequal  apint a & apint b
template <int I_>
bool generic_isequal(ap_int<I_> x,ap_int<I_> y)
{
#pragma HLS pipeline
    return (x==y);
}

///isnotequal  apint a & apint b
template <int I_>
bool generic_isnotequal(ap_int<I_> x,ap_int<I_> y)
{
#pragma HLS pipeline
    return (!(x==y));
}

///isequal  ap_uint a & ap_int b
template <int I_>
bool generic_isequal(ap_uint<I_> x,ap_int<I_> y)
{
#pragma HLS pipeline
	return (x==y);
}

///isnotequal  ap_uint a & ap_int b
template <int I_>
bool generic_isnotequal(ap_uint<I_> x,ap_int<I_> y)
{
#pragma HLS pipeline
	return (!(x==y));
}

///isequal  ap_int a & ap_uint b
template <int I_>
bool generic_isequal(ap_int<I_> x,ap_uint<I_> y)
{
#pragma HLS pipeline
	return (x==y);
}

///isnotequal  ap_int a & ap_uint b
template <int I_>
bool generic_isnotequal(ap_int<I_> x,ap_uint<I_> y)
{
#pragma HLS pipeline
	return (!(x==y));
}

///isequal  ap_uint a & ap_uint b
template <int I_>
bool generic_isequal(ap_uint<I_> x,ap_uint<I_> y)
{
#pragma HLS pipeline
	return (x==y);
}

///isnotequal  ap_uint a & ap_uint b
template <int I_>
bool generic_isnotequal(ap_uint<I_> x,ap_uint<I_> y)
{
#pragma HLS pipeline
	return (!(x==y));
}

#endif



