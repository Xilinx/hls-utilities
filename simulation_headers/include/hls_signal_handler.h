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

#include <signal.h>
#include <string>
#include <iostream>

enum CodeStateE {ENTER_WRAPC, DUMP_INPUTS, CALL_C_DUT, DUMP_OUTPUTS, DELETE_CHAR_BUFFERS, ENTER_WRAPC_PC} CodeState;
std::string CodeStateS[] = {"ENTER_WRAPC", "DUMP_INPUTS", "CALL_C_DUT", "DUMP_OUTPUTS", "DELETE_CHAR_BUFFERS", "ENTER_WRAPC_PC"};

void message_handler (int sig) 
{
    if (sig == SIGFPE)
    {
        std::cout << "ERROR: System received a signal named SIGFPE and the program has to stop immediately!" << "\n"
        << "This signal was generated due to a fatal arithmetic error." << "\n"
        << "Possible cause of this problem may be: division by zero, overflow etc." << "\n"
        << "Current execution stopped during CodeState = " << CodeStateS[CodeState] << "." << "\n"
        << "You can search CodeState variable name in apatb*.cpp file under ./sim/wrapc dir to locate the position." << "\n"
        << std::endl;
    }
    if (sig == SIGSEGV)
    {
        std::cout << "ERROR: System received a signal named SIGSEGV and the program has to stop immediately!\n"
                     "This signal was generated when a program tries to read or write outside the memory that is allocated for it, "
                     "or to write memory that can only be read.\n"
                     "Possible cause of this problem may be:\n"
                     "  1) Missing depth information for one or more pointers to arrays in the interface;\n"
                     "  2) Insufficient depth setting for array argument(s);\n"
                     "  3) Excessive depth setting for array argument(s), that exceeds the maximum virtual memory size for the process;\n"
                     "  4) Null pointer etc.\n"
                  << "Current execution stopped during CodeState = " << CodeStateS[CodeState] << ".\n"
                  << "You can search CodeState variable name in apatb*.cpp file under ./sim/wrapc dir to locate the position.\n"
                  << std::endl;
    }
    if (sig == SIGTERM)
    {
        std::cout << "ERROR: System received a signal named SIGTERM and the program has to stop immediately!" << "\n"
        << "This signal was caused by the shell command kill." << "\n"
        << "Current execution stopped during CodeState = " << CodeStateS[CodeState] << "." << "\n"
        << "You can search CodeState variable name in apatb*.cpp file under ./sim/wrapc dir to locate the position." << "\n"
        << std::endl;
    }
    if (sig == SIGINT)
    {
        std::cout << "ERROR: System received a signal named SIGINT and the program has to stop immediately!" << "\n"
        << "This signal was generated when the user types the INTR character (normally C-c)." << "\n"
        << "Current execution stopped during CodeState = " << CodeStateS[CodeState] << "." << "\n"
        << "You can search CodeState variable name in apatb*.cpp file under ./sim/wrapc dir to locate the position." << "\n"
        << std::endl;
    }
//#ifdef _MINGW32_
//#else
//    if (sig == SIGKILL)
//    {
//        std::cout << "ERROR: System received a signal named SIGKILL and the program has to stop immediately!" << "\n"
//        << "The system generated SIGKILL for a process itself under some unusual conditions where the program cannot possibly continue to run." << "\n"
//        << "Current execution stopped during CodeState = " << CodeStateS[CodeState] << "." << "\n"
//        << "You can search CodeState variable name in apatb*.cpp file under ./sim/wrapc dir to locate the position." << "\n"
//        << "\n";
//    }
//    if (sig == SIGQUIT)
//    {
//        std::cout << "ERROR: System received a signal named SIGQUIT and the program has to stop immediately!" << "\n"
//        << "This signal was generated when the user types the QUIT character and produces a core dump." << "\n"
//        << "Current execution stopped during CodeState = " << CodeStateS[CodeState] << "." << "\n"
//        << "You can search CodeState variable name in apatb*.cpp file under ./sim/wrapc dir to locate the position." << "\n"
//        << "\n";
//    }
//#endif
    signal(sig, SIG_DFL);
}

extern "C" void refine_signal_handler() 
{
    signal(SIGFPE, message_handler);
    signal(SIGSEGV, message_handler);
    signal(SIGTERM, message_handler);
    signal(SIGINT, message_handler);
//#ifdef _MINGW32_
//#else
//    signal(SIGKILL, message_handler);
//    signal(SIGQUIT, message_handler);
//#endif
}
