/*!
    \file    main.h
    \brief   the header file of main

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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

#ifndef MAIN_H
#define MAIN_H
#include <arm_cmse.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#define CMSE_NS_CALL  __cmse_nonsecure_call
#define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif /* __ICCARM__ */

#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *ns_fptr)(void);
#else
typedef void CMSE_NS_CALL (*ns_fptr)(void);
#endif /* __ICCARM__ */

#define TZBMPC1_BLOCK_NUMBER    255                          /* TZBMPC1 block number */

#define NONSECURE_START        ((uint32_t)0x08040000U)       /* nonsecure start address */
#define SECM_SPAGE             ((uint32_t)0x00000000U)       /* start page of mark    */
#define SECM_EPAGE             ((uint32_t)0x0000003FU)       /* end page of mark      */

void fmc_trustzone_enable(void);
void fmc_secmark_config(void);
void tzbmpc_config();

void led_config(void);
void com_config(void);

void nonsecure_init();
CMSE_NS_ENTRY void entry_cb_func_register(void *callback);
CMSE_NS_ENTRY void non_secure_print(const char * str);

#endif /* MAIN_H */


