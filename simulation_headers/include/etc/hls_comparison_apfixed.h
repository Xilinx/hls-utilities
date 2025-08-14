// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

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


