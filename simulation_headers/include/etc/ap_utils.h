// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef _AUTOPILOT_UTILS_H_
#define _AUTOPILOT_UTILS_H_


#ifndef __SYNTHESIS__
#include <string.h>
#include <stdio.h>
#endif

#ifdef __CYGWIN__
#  ifdef feof
#    undef feof
#  endif

#  ifdef ferror
#    undef ferror
#  endif
#endif


#include "etc/autopilot_ssdm_op.h"


#endif /* #ifndef _AUTOPILOT_UTILS_H_ */


