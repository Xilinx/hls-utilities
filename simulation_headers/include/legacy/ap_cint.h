// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

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


