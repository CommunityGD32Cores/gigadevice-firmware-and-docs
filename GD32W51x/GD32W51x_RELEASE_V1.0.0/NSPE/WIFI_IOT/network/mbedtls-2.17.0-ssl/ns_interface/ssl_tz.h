/*!
    \file    ssl_tz.h
    \brief   SSL trustzone definition for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef __SSL_TZ__H
#define __SSL_TZ__H
//#include "platform_def.h"
#include "mbedtls/config.h"

/*
* If TrustZone is enable, SSL tls handshake and network sockets operation are in the side of NSPE.
* And mbedtls symbols like crypto, public key, x509 etc. are in the side of SPE.
* So it needs to impletment NSC funtion for SSL call ROM symbol
*/
#if defined(CONFIG_TZ_ENABLED)
/*
* Multi-defined symbol in rom symbols and implements.
* This header files would rename some symbols to call the real implement symbol.
*/
#include "mbedtls_se_api_def.h"

/*
* Some mebedtls crypto api that called by SSL coudle be replace by PSA crypto except mbedtls_cipher_xxx
* This marco make difference from MBEDTLS_USE_PSA_CRYPTO
*/
#define SSL_USE_PSA_CRYPTO

#define SSL_USE_PSA_CRYPTO_MAC

#define SSL_USE_MBEDTLS_NSC

/* Platform API should use NPSE implemention */
#include "wrapper_os.h"
#define MBEDTLS_PLATFORM_FREE_MACRO		sys_mfree
#define MBEDTLS_PLATFORM_CALLOC_MACRO	sys_calloc

#undef MBEDTLS_PLATFORM_PRINTF_ALT
#define MBEDTLS_PLATFORM_PRINTF_MACRO	printf

#undef MBEDTLS_PLATFORM_SNPRINTF_ALT
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO	snprintf

#undef MBEDTLS_PLATFORM_TIME_ALT

#define MBEDTLS_PLATFORM_TIME_MACRO		sys_time_get


#endif  // CONFIG_TZ_ENABLED

#define GD_MODIFIED

#ifdef GD_MODIFIED
#define MAX_OPERANDS_RSA_HW_ENGINE      3136
#define MAX_OPERANDS_ECC_HW_ENGINE      640
#endif

#endif  // __SSL_TZ__H
