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


#ifndef _X_HLS_COMPARISON_APFIXED_H_
#define _X_HLS_COMPARISON_APFIXED_H_
#include "ap_fixed.h"
#include "ap_int.h"


//isgreater(ap_fixed)
template<int W, int I>
bool  isgreater_fixed(ap_fixed<W,I> x1, ap_fixed<W,I> x2){
    if(x1 > x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int W, int I>
bool  isgreater_fixed(ap_ufixed<W,I> x1, ap_ufixed<W,I> x2){
    if(x1 > x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  isgreater_fixed(ap_int<I> x1, ap_int<I> x2){
    if(x1 > x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  isgreater_fixed(ap_uint<I> x1, ap_uint<I> x2){
    if(x1 > x2) {
        return true;
    }
    else {
        return false;
    }
}

//isgreaterequal(ap_fixed)
template<int W, int I>
bool  isgreaterequal_fixed(ap_fixed<W,I> x1, ap_fixed<W,I> x2){
    if(x1 >= x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int W, int I>
bool  isgreaterequal_fixed(ap_ufixed<W,I> x1, ap_ufixed<W,I> x2){
    if(x1 >= x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  isgreaterequal_fixed(ap_int<I> x1, ap_int<I> x2){
    if(x1 >= x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  isgreaterequal_fixed(ap_uint<I> x1, ap_uint<I> x2){
    if(x1 >= x2) {
        return true;
    }
    else {
        return false;
    }
}

//isless(ap_fixed)
template<int W, int I>
bool  isless_fixed(ap_fixed<W,I> x1, ap_fixed<W,I> x2){
    if(x1 < x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int W, int I>
bool  isless_fixed(ap_ufixed<W,I> x1, ap_ufixed<W,I> x2){
    if(x1 < x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  isless_fixed(ap_int<I> x1, ap_int<I> x2){
    if(x1 < x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  isless_fixed(ap_uint<I> x1, ap_uint<I> x2){
    if(x1 < x2) {
        return true;
    }
    else {
        return false;
    }
}

//islessequal(ap_fixed)
template<int W, int I>
bool  islessequal_fixed(ap_fixed<W,I> x1, ap_fixed<W,I> x2){
    if(x1 <= x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int W, int I>
bool  islessequal_fixed(ap_ufixed<W,I> x1, ap_ufixed<W,I> x2){
    if(x1 <= x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  islessequal_fixed(ap_int<I> x1, ap_int<I> x2){
    if(x1 <= x2) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  islessequal_fixed(ap_uint<I> x1, ap_uint<I> x2){
    if(x1 <= x2) {
        return true;
    }
    else {
        return false;
    }
}

//islessgreater(ap_fixed)
template<int W, int I>
bool  islessgreater_fixed(ap_fixed<W,I> x1, ap_fixed<W,I> x2){
    if((x1 < x2) || (x1 > x2)) {
        return true;
    }
    else {
        return false;
    }
}
template<int W, int I>
bool  islessgreater_fixed(ap_ufixed<W,I> x1, ap_ufixed<W,I> x2){
    if((x1 < x2) || (x1 > x2)) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  islessgreater_fixed(ap_int<I> x1, ap_int<I> x2){
    if((x1 < x2) || (x1 > x2)) {
        return true;
    }
    else {
        return false;
    }
}
template<int I>
bool  islessgreater_fixed(ap_uint<I> x1, ap_uint<I> x2){
    if((x1 < x2) || (x1 > x2)) {
        return true;
    }
    else {
        return false;
    }
}




#endif


