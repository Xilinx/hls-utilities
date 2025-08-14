// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

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



