/*!
    \file  main.h
    \brief the header file of main 

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2019-04-01, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f20x.h"
#include "stdio.h"

#ifndef __MAIN_H
#define __MAIN_H

#define HASH_MODE               "HASH"
#define HMAC_MODE               "HMAC"
#define SHA1                    "SHA1"
#define SHA224                  "SHA224"
#define SHA256                  "SHA256"
#define MD5                     "MD5"

#define ALGORITHM_USE_SHA1      0x01
#define ALGORITHM_USE_SHA224    0x02
#define ALGORITHM_USE_SHA256    0x04
#define ALGORITHM_USE_MD5       0x08
#define MODE_USE_HASH           0x10
#define MODE_USE_HMAC           0x20

const uint8_t message_input[]= 
"The GD32 F2 series is the result of a perfect symbiosis of the real-time control capabilities of an MCU and the signal processing performance of a DSP,\
and thus complements the GD32 portfolio with a new class of devices, digital signal controllers (DSC).";

const uint8_t key[] = "The hash processor is a fully compliant implementation of the secure hash algorithm (SHA-1), the MD5 (message-digest algorithm 5) hash\
algorithm and the HMAC (keyed-hash message authentication code) algorithm suitable for a variety of applications.";

#endif /* __MAIN_H */
