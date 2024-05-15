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


#ifndef _AUTOPILOT_APINT_H_
#define _AUTOPILOT_APINT_H_

#include "etc/autopilot_dt.h"
#include "etc/autopilot_ssdm_bits.h"

#define APInt_BitWidthOf(X) _ssdm_op_bitwidthof(X)

#define APInt_Concatenate(X, Y) _ssdm_op_Concat(X, Y)
#define APInt_GetBit(Val, Bit) _ssdm_op_GetBit(Val, Bit)
#define APInt_SetBit(Val, Bit, Repl) _ssdm_op_SetBit(Val, Bit, Repl)
#define APInt_GetRange(Val, Hi, Lo) _ssdm_op_GetRange(Val, Hi, Lo)
#define APInt_SetRange(Val, Hi, Lo, Repl) _ssdm_op_SetRange(Val, Hi, Lo, Repl)

#define APInt_AndReduce(X) _ssdm_op_reduce(and, X)
#define APInt_NandReduce(X) _ssdm_op_reduce(nand, X)
#define APInt_OrReduce(X) _ssdm_op_reduce(or, X)
#define APInt_NorReduce(X) _ssdm_op_reduce(nor, X)
#define APInt_XorReduce(X) _ssdm_op_reduce(xor, X)
#define APInt_XnorReduce(X) _ssdm_op_reduce(nxor, X)

#define APInt_BitsFromString(Str, Bits) \
    _ssdm_op_bitsFromString(Str, Bits)
#define APInt_BitsFromHexString(Str, Bits) \
    _ssdm_op_bitsFromHexString(Str, Bits)
#define APInt_BitsFromOctalString(Str, Bits) \
    _ssdm_op_bitsFromOctalString(Str, Bits)
#define APInt_BitsFromBinaryString(Str, Bits) \
    _ssdm_op_bitsFromBinaryString(Str, Bits)


/************************************************/

#define apint_bitwidthof(X) _ssdm_op_bitwidthof(X)

#define apint_concatenate(X, Y) _ssdm_op_Concat(X, Y)
#define apint_get_bit(Val, Bit) _ssdm_op_GetBit(Val, Bit)
#define apint_set_bit(Val, Bit, Repl) _ssdm_op_SetBit(Val, Bit, Repl)
#define apint_get_range(Val, Hi, Lo) _ssdm_op_GetRange(Val, Hi, Lo)
#define apint_set_range(Val, Hi, Lo, Repl) _ssdm_op_SetRange(Val, Hi, Lo, Repl)

#define apint_and_reduce(X) _ssdm_op_reduce(and, X)
#define apint_nand_reduce(X) _ssdm_op_reduce(nand, X)
#define apint_or_reduce(X) _ssdm_op_reduce(or, X)
#define apint_nor_reduce(X) _ssdm_op_reduce(nor, X)
#define apint_xor_reduce(X) _ssdm_op_reduce(xor, X)
#define apint_xnor_reduce(X) _ssdm_op_reduce(nxor, X)

#define apint_print(Val, Radix) _ssdm_op_print_apint(Val, Radix)
#define apint_fprint(FileOut, Val, Radix) _ssdm_op_fprint_apint(FileOut, Val, Radix)

#define apint_vstring2bits_hex(Str, Bits) _ssdm_op_sscan_hex_apint(Str, Bits)
#define apint_string2bits_dec(Str, Bits) _ssdm_op_bitsFromString(Str, Bits)
#define apint_string2bits_hex(Str, Bits) _ssdm_op_bitsFromHexString(Str, Bits)
#define apint_string2bits_oct(Str, Bits) _ssdm_op_bitsFromOctalString(Str, Bits)
#define apint_string2bits_bin(Str, Bits) _ssdm_op_bitsFromBinaryString(Str, Bits)
#define apint_string2bits(Str, Bits) apint_string2bits_dec(Str, Bits)

#endif


