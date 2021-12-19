/*!
    \file    gd32w51x_it.c
    \brief   interrupt service routines

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "gd32w51x_it.h"
#include "main.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this is SVC handle function, get SVC value and set control register
    \param[in]  svc_args: stack address
    \param[in]  exc_return: LR address
    \param[out] none
    \retval     exc_return: LR address
*/
uint32_t svc_handler(uint32_t *svc_args, uint32_t exc_return)
{
    uint8_t svc_value;
    uint32_t control_value;
    /*
      Stack contains:
      r0, r1, r2, r3, r12, r14 (lr), the return address and xPSR
      First argument (r0) is svc_args[0]
    */
    svc_value = ((uint8_t *)svc_args[6])[-2];

    control_value = __get_CONTROL();
    /* configure privilege level */
    switch(svc_value){
    case PRIV_MODE:
        __set_CONTROL(control_value & 0xFFFFFFFEU);
        break;
    case NPRIV_MODE:
        __set_CONTROL(control_value | 0x00000001U);
        break;
    default:
        break;
    }
    return exc_return;
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    __ASM volatile(
    "MRS     r2, MSP                        \n"
    /* Check store SP in thread mode to r0 */
    "TST     lr, #4                         \n"
    "ITE     EQ                             \n"
    "MOVEQ   r0, r2                         \n"
    "MRSNE   r0, PSP                        \n"
    "MOV     r1, lr                         \n"
    "BL      svc_handler                    \n"
    "BX      r0                             \n"
    );
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SecureFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SecureFault_Handler(void)
{
    /* if Secure Fault exception occurs, go to infinite loop */
    while(1){
    }
}
