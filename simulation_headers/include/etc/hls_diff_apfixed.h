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


#ifndef _X_HLS_DIFF_APFIXED_H_
#define _X_HLS_DIFF_APFIXED_H_
#include "ap_fixed.h"
#include "ap_int.h"

//1. fdim_fixed
 /*
  * ============================================
  * ||     x     y            ||      fdim_fixed(x,y)
  * ||     (x-y) Normally     ||      (x  -  y)
  * ||     (x-y) overflow     ||         max
  * ||     x  <  y            ||          0
  * ============================================
  */
template <int W_, int I_>
ap_fixed<W_,I_> fdim_fixed(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y )
{
    #pragma HLS PIPELINE II=1
    ap_fixed<W_+1,I_+1>  x_overf = 0;
    ap_fixed<W_ ,I_ >         xs = 0;   
        bool               overf = 0;
    
    if(x > y)
    {
        x_overf = x - y;
        overf =  x_overf[W_-1];
        if(overf) { 
            for (int i = 0; i < W_-1; i++){            
            #pragma HLS UNROLL        
                xs[i] = 1;
            }
            xs[W_-1] = 0;
        }
        else{
            x_overf[W_] = 0;            
            xs = x_overf;           
        }
    }   
    return xs;
}
template <int W_, int I_>
ap_ufixed<W_,I_> fdim_fixed(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y)
{
    ap_ufixed<W_ ,I_ > xs = 0;  
    if(x > y){ xs = x - y; }       
    return xs;
}
template <int I_>
ap_int<I_> fdim_fixed(ap_int<I_> x, ap_int<I_> y)
{
    ap_fixed<I_,I_> xs = x;
    ap_fixed<I_,I_> ys = y; 
    return fdim_fixed(xs, ys);
}
template <int I_>
ap_uint<I_> fdim_fixed(ap_uint<I_> x, ap_uint<I_> y)
{
    ap_uint<I_ >       xs = 0;  
    if(x > y) { xs = x - y; }       
    return xs;
}

//2. fmax_fixed
 /*
  * ============================================
  * ||     x        y         ||      fmax_fixed(x,y)
  * ||     x    >=  y         ||             x  
  * ||     x    <   y         ||             y
  * ============================================
  */
template <int W_, int I_>
ap_fixed<W_,I_> fmax_fixed(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y )
{
    ap_fixed<W_ ,I_ > xs = 0;   
    if(x >= y) xs = x ;
        else   xs = y ;
    return xs;
}
template <int W_, int I_>
ap_ufixed<W_,I_> fmax_fixed(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y)
{
    ap_ufixed<W_ ,I_ > xs = 0;  
    if(x >= y) xs = x;
        else   xs = y ; 
    return xs;
}
template <int I_>
ap_int<I_> fmax_fixed(ap_int<I_> x, ap_int<I_> y)
{
    ap_int<I_ >        xs = 0;  
    if(x >= y) xs = x;
        else   xs = y ; 
    return xs;
}
template <int I_>
ap_uint<I_> fmax_fixed(ap_uint<I_> x, ap_uint<I_> y)
{
    ap_uint<I_ >       xs = 0;  
    if(x >= y) xs = x;
        else   xs = y ; 
    return xs;
}

//3. fmin_fixed
 /*
  * ============================================
  * ||     x        y         ||      fmin_fixed(x,y)
  * ||     x    >   y         ||             y  
  * ||     x    <=  y         ||             x
  * ============================================
  */
template <int W_, int I_>
ap_fixed<W_,I_> fmin_fixed(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y )
{
    ap_fixed<W_ ,I_ > xs = 0;   
    if(x <= y) xs = x ;
        else   xs = y ;
    return xs;
}
template <int W_, int I_>
ap_ufixed<W_,I_> fmin_fixed(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y)
{
    ap_ufixed<W_ ,I_ > xs = 0;  
    if(x <= y) xs = x;
        else   xs = y ; 
    return xs;
}
template <int I_>
ap_int<I_> fmin_fixed(ap_int<I_> x, ap_int<I_> y)
{
    ap_int<I_ >        xs = 0;  
    if(x <= y) xs = x;
        else   xs = y ; 
    return xs;
}
template <int I_>
ap_uint<I_> fmin_fixed(ap_uint<I_> x, ap_uint<I_> y)
{
    ap_uint<I_ >       xs = 0;  
    if(x <= y) xs = x;
        else   xs = y ; 
    return xs;
}

// TODO DEBUG
//4. maxmag_fixed
 /*
  * ============================================
  * ||     x        y         ||      maxmag_fixed(x,y)
  * || max{|x|,|y|} overflow  ||            max  
  * ||    |x|   >=  |y|       ||             x  
  * ||    |x|   <   |y|       ||             y
  * ============================================
  */
template <int W_, int I_>
ap_fixed<W_,I_> maxmag_fixed(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y )
{
    #pragma HLS PIPELINE II=1
    ap_fixed<W_+1,I_+1> xm = -x;    
    ap_fixed<W_+1,I_+1> ym = -y;
    ap_fixed<W_+1,I_+1> x_fabs = ( x[W_-1] ) ? xm : ap_fixed<W_+1,I_+1>(x);
    ap_fixed<W_+1,I_+1> y_fabs = ( y[W_-1] ) ? ym : ap_fixed<W_+1,I_+1>(y);
    
    ap_fixed<W_+1,I_+1>   xs_t = 0;
    ap_fixed<W_,I_>         xs = 0;
    if(x_fabs >= y_fabs)
    {
    	xs_t = x_fabs ;
    	xs   = x;
    }
    else{
    	xs_t = y_fabs ;
    	xs   = y;
    }
    //case overflow
    if(xs_t[W_-1])
    {
            for (int i = 0; i < W_-1; i++){
            #pragma HLS UNROLL        
                xs[i] = 1;
            }
            xs[W_-1] = 0;
    }

    return xs;
}
template <int W_, int I_>
ap_ufixed<W_,I_> maxmag_fixed(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y)
{
    ap_ufixed<W_ ,I_ > xs = 0;  
    if(x >= y) xs = x;
        else   xs = y ; 
    return xs;
}
template <int I_>
ap_int<I_> maxmag_fixed(ap_int<I_> x, ap_int<I_> y)
{
    ap_fixed<I_,I_> xs = x;
    ap_fixed<I_,I_> ys = y; 
    return maxmag_fixed(xs, ys);
}
template <int I_>
ap_uint<I_> maxmag_fixed(ap_uint<I_> x, ap_uint<I_> y)
{
    ap_uint<I_ >       xs = 0;  
    if(x >= y) xs = x;
        else   xs = y ; 
    return xs;
}


//5. minmag_fixed
 /*
  * ============================================
  * ||     x        y         ||      minmag_fixed(x,y)
  * || min{|x|,|y|} overflow  ||            max 
  * ||    |x|   >  |y|        ||             y  
  * ||    |x|  <=  |y|        ||             x
  * ============================================
  */
template <int W_, int I_>
ap_fixed<W_,I_> minmag_fixed(ap_fixed<W_,I_> x, ap_fixed<W_,I_> y )
{
    #pragma HLS PIPELINE II=1
    ap_fixed<W_+1,I_+1> xm = -x;    
    ap_fixed<W_+1,I_+1> ym = -y;
    ap_fixed<W_+1,I_+1> x_fabs = ( x[W_-1] ) ? xm : ap_fixed<W_+1,I_+1>(x);
    ap_fixed<W_+1,I_+1> y_fabs = ( y[W_-1] ) ? ym : ap_fixed<W_+1,I_+1>(y);
    
    ap_fixed<W_+1,I_+1>   xs_t = 0;
    ap_fixed<W_,I_>         xs = 0;
    if(x_fabs <= y_fabs)
    {
    	xs_t = x_fabs ;
    	xs   = x;
    }
    else{
    	xs_t = y_fabs ;
    	xs   = y;
    }
    if(xs_t[W_-1])
    {
            for (int i = 0; i < W_-1; i++){           
            #pragma HLS UNROLL        
                xs[i] = 1;
            }
            xs[W_-1] = 0;
    }

    return xs;
}
template <int W_, int I_>
ap_ufixed<W_,I_> minmag_fixed(ap_ufixed<W_,I_> x, ap_ufixed<W_,I_> y)
{
    ap_ufixed<W_ ,I_ > xs = 0;  
    if(x <= y) xs = x;
        else   xs = y ; 
    return xs;
}
template <int I_>
ap_int<I_> minmag_fixed(ap_int<I_> x, ap_int<I_> y)
{
    ap_fixed<I_,I_> xs = x;
    ap_fixed<I_,I_> ys = y; 
    return minmag_fixed(xs, ys);
}
template <int I_>
ap_uint<I_> minmag_fixed(ap_uint<I_> x, ap_uint<I_> y)
{
    ap_uint<I_ >      xs = 0;  
    if(x <= y) xs = x;
        else   xs = y ; 
    return xs;
}
#endif


