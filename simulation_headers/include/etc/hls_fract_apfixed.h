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

#ifndef _X_HLS_GENERIC_FRACT_H_
#define _X_HLS_GENERIC_FRACT_H_

#include "utils/x_hls_defines.h"
#include "utils/x_hls_utils.h"
#include "utils/x_hls_traits.h"

namespace hls_fract {
//fract for ap_fixed
template <int W_, int I_>
ap_fixed<W_,I_> generic_fract(ap_fixed<W_,I_> x){
	if (W_==I_)return 0;
	ap_fixed<W_-I_+1,1> x1=x;
	ap_fixed<W_-I_+1,1> x2=x1+ap_int<2>(1);
	return x[W_-1]?x2:x1;
}
//fract for ap_ufixed
template <int W_, int I_>
ap_ufixed<W_,I_> generic_fract(ap_ufixed<W_,I_> x){
	ap_fixed<W_+1,I_+1> x1=x;
	return generic_fract(x1);
}
//fract for ap_int
template <int I_>
ap_int<I_> generic_fract(ap_int<I_> x){
	ap_fixed<I_,I_> x1 = x;
	return generic_fract(x1);
}
//fract for ap_uint
template <int I_>
ap_uint<I_> generic_fract(ap_uint<I_> x){
	ap_ufixed<I_,I_> x1 = x;
	return generic_fract(x1);
}
}
#endif


