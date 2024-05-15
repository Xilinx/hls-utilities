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



/**
 * @file hls_fmod_rem_quo.h
 *
 */
#ifndef _X_HLS_FMOD_REM_QUO_H_
#define _X_HLS_FMOD_REM_QUO_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "etc/hls_round_copysign_apfixed.h"

namespace hls_internal{
    //divide for ap_fixed numerator and denominator
    template <int W_, int I_>
    ap_fixed<W_,I_> generic_divide(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y){
        if(y==0){//case y==0, return error and stop
            #ifndef __SYNTHESIS__
                printf("ERROR:hls_math.h:divisor cannot be zero");
	        exit(1);
            #else
                return 0;
            #endif
        } else{
	    return(x/y);
        }
    }
    //divide for ap_ufixed numerator and denominator
    template <int W_, int I_>
    ap_ufixed<W_,I_> generic_divide(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y){
    	ap_fixed<W_+1,I_+1> x1 = x;
    	ap_fixed<W_+1,I_+1> y1 = y;
    	return generic_divide(x1,y1);
    }
    //divide for ap_int
    template <int I_>
    ap_int<I_> generic_divide(ap_int<I_> x, ap_int<I_> y){
    	ap_fixed<I_,I_> x1 = x;
    	ap_fixed<I_,I_> y1 = y;
    	return generic_divide(x1,y1);
    }
    //divide for ap_uint
    template <int I_>
    ap_uint<I_> generic_divide(ap_uint<I_> x, ap_uint<I_> y){
    	ap_ufixed<I_,I_> x1 = x;
    	ap_ufixed<I_,I_> y1 = y;
    	return generic_divide(x1,y1);
    }
    //fmod for ap_fixed
    template <int W_, int I_>
    ap_fixed<W_,I_> generic_fmod(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y){
        ap_ufixed<W_,I_> x_pos = fabs_fixed((ap_fixed<W_+1,I_+1>)x);
        ap_ufixed<W_,I_> y_pos = fabs_fixed((ap_fixed<W_+1,I_+1>)y);
        //std::cout<<"x_pos"<<x_pos<<std::endl;
        //std::cout<<"y_pos"<<y_pos<<std::endl;
        if(x_pos==0 || y_pos==0) return 0;
    	else if(x_pos == y_pos)  return 0;
        else if(x_pos < y_pos)   return x;
        else {
            ap_uint<W_> x_int = 0;
            ap_uint<W_> y_int = 0;
            x_int(W_-1,0) = x_pos(W_-1,0);
            y_int(W_-1,0) = y_pos(W_-1,0);
            //use less resource 
            //for(int i = 0; i < W_; ++i) {
            //    if(x_int > y_int) {
            //        x_int -= y_int;
            //    }
            //} 
            ap_uint<W_> d = x_int/y_int;
            //std::cout<<"d="<<d<<std::endl;
            ap_uint<W_> rem = x_int - d*y_int;
            //std::cout<<"rem="<<rem<<std::endl;
            ap_fixed<W_,I_> r = 0;
            r(W_-1,0) = rem(W_-1,0);
            if(x[W_-1]) return -r;
            else        return r;
        }
    }
    
    //fmod for ap_ufixed
    template <int W_, int I_>
    ap_ufixed<W_,I_> generic_fmod(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y){
        ap_fixed<W_+1,I_+1> xi = x;
        ap_fixed<W_+1,I_+1> yi = y;
        return generic_fmod(xi,yi);
    }
    //fmod for ap_int
    template <int I_>
    ap_int<I_> generic_fmod(ap_int<I_> x, ap_int<I_> y){
    	ap_fixed<I_,I_> xi = x;
    	ap_fixed<I_,I_> yi = y;
    	return generic_fmod(xi,yi);
    }
    //fmod for ap_uint
    template <int I_>
    ap_uint<I_> generic_fmod(ap_uint<I_> x, ap_uint<I_> y){
    	ap_fixed<I_+1,I_+1> xi = x;
    	ap_fixed<I_+1,I_+1> yi = y;
    	return generic_fmod(xi,yi);
    }

    //remquo for ap_fixed
    template <int W_, int I_>
    ap_fixed<W_,I_> generic_remquo(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y, int* quo){
        ap_ufixed<W_,I_> x_pos = fabs_fixed((ap_fixed<W_+1,I_+1>)x);
        ap_ufixed<W_,I_> y_pos = fabs_fixed((ap_fixed<W_+1,I_+1>)y);
        if(x_pos==0 || y_pos==0) {
           *quo = 0;
           return 0;
        } else if(x_pos == y_pos) {
           if(x[W_-1]==y[W_-1]) *quo = 1;
           else                 *quo = -1;
           return 0;
        }
        //else if(x_pos < y_pos) return x;
        else {
            ap_uint<W_> x_int = 0;
            ap_uint<W_> y_int = 0;
            x_int(W_-1,0) = x_pos(W_-1,0);
            y_int(W_-1,0) = y_pos(W_-1,0);
            //for(int i = 0; i < W_; ++i) {
            //    if(x_int > y_int) {
            //        x_int -= y_int;
            //    }
            //} 
            ap_uint<W_> d = x_int/y_int;
            ap_uint<W_> rem = x_int - d*y_int;
            //adjust the result
            ap_uint<W_> y_half = 0;
            y_half(W_-2,0) = y_int(W_-1,1);
            ap_uint<1> r_sig = x[W_-1];
            if(rem > y_half) {
                rem = y_int - rem;
                d++;
                if(x[W_-1]) r_sig = 0;
                else        r_sig = 1;
            }
            //return the results 
            if(x[W_-1]==y[W_-1]) *quo = d;
            else                 *quo = -d;
            ap_fixed<W_,I_> r = 0;
            r(W_-1,0) = rem(W_-1,0);
            if(r_sig) return -r;
            else      return r;
        }
    }
    //, y, quoremquo for ap_ufixed
    template <int W_, int I_>
    ap_ufixed<W_,I_> generic_remquo(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y, int* quo){
    	ap_fixed<W_+1,I_+1> xi = x;
    	ap_fixed<W_+1,I_+1> yi = y;
    	return generic_remquo(xi,yi,quo);
    }
    //remquo for ap_int
    template <int I_>
    ap_int<I_> generic_remquo(ap_int<I_> x, ap_int<I_> y, int* quo){
    	ap_fixed<I_,I_> xi = x;
    	ap_fixed<I_,I_> yi = y;
    	return generic_remquo(xi,yi,quo);
    }
    //remquo for ap_uint
    template <int I_>
    ap_uint<I_> generic_remquo(ap_uint<I_> x, ap_uint<I_> y, int* quo){
    	ap_fixed<I_+1,I_+1> xi = x;
    	ap_fixed<I_+1,I_+1> yi = y;
    	return generic_remquo(xi,yi,quo);
    }

    //remainder for ap_fixed
    template <int W_, int I_>
    ap_fixed<W_,I_> generic_remainder(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y){
        int quo = 0;
        return generic_remquo(x, y, &quo);
    }
    //remainder for ap_ufixed
    template <int W_, int I_>
    ap_ufixed<W_,I_> generic_remainder(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y){
        ap_fixed<W_+1, I_+1> xi = x;
        ap_fixed<W_+1, I_+1> yi = y;
        return generic_remainder(xi,yi);
    }
    //remainder for ap_int
    template <int I_>
    ap_int<I_> generic_remainder(ap_int<I_> x, ap_int<I_> y){
    	ap_fixed<I_,I_> xi = x;
    	ap_fixed<I_,I_> yi = y;
    	return generic_remainder(xi,yi);
    }
    //remainder for ap_uint
    template <int I_>
    ap_uint<I_> generic_remainder(ap_uint<I_> x, ap_uint<I_> y){
    	ap_fixed<I_+1,I_+1> xi = x;
    	ap_fixed<I_+1,I_+1> yi = y;
    	return generic_remainder(xi,yi);
    }
}
#endif // entire file

