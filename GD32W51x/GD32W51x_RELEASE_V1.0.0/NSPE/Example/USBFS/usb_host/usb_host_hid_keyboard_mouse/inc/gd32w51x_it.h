/*!
    \file    gd32w51x_it.h
    \brief   the header file of the ISR

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

#ifndef GD32W51x_IT_H
#define GD32W51x_IT_H

#include "usb_conf.h"

/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles external line 1 interrupt request */
void EXTI1_IRQHandler(void);
/* this function handles hard fault exception. */
void HardFault_Handler(void);
/* this function handles memory manage exception */
void MemManage_Handler(void);
/* this function handles bus fault exception */
void BusFault_Handler(void);
/* this function handles usage fault exception */
void UsageFault_Handler(void);
/* this function handles SVCall exception */
void SVC_Handler(void);
/* this function handles Debug Monitor exception */
void DebugMon_Handler(void);
/* this function handles PendSVC exception */
void PendSV_Handler(void);
/* this function handles SysTick Handler */
void SysTick_Handler(void);
/* this function handles Timer2 Handler */
void TIMER2_IRQHandler(void);
/* this function handles USBHS global interrupt request */
void USBHS_IRQHandler(void);

#endif /* GD32w51X_IT_H */
