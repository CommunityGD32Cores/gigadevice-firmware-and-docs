/*!
    \file    gd32w51x_it.c
    \brief   interrupt service routines

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

#include "gd32w51x_it.h"
#include "rom_export.h"

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
    \brief      this function print core register information
    \param[in]  stack: store core register value
    \param[out] none
    \retval     none
*/
void HardFault_Handler_c(unsigned int stack[])
{
    rom_printf("[Hard Fault Handler]\r\n");
    rom_printf("    R0 = 0x%08X\r\n", stack[0]);
    rom_printf("    R1 = 0x%08X\r\n", stack[1]);
    rom_printf("    R2 = 0x%08X\r\n", stack[2]);
    rom_printf("    R3 = 0x%08X\r\n", stack[3]);
    rom_printf("    R12 = 0x%08X\r\n", stack[4]);
    rom_printf("    LR = 0x%08X\r\n", stack[5]);
    rom_printf("    PC = 0x%08X\r\n", stack[6]);
    rom_printf("    PSR = 0x%08X\r\n", stack[7]);
    rom_printf("    BFAR = 0x%08X\r\n", SCB->BFAR);
    rom_printf("    CFSR = 0x%08X\r\n", SCB->CFSR);
    rom_printf("    HFSR = 0x%08X\r\n", SCB->HFSR);
    rom_printf("    DFSR = 0x%08X\r\n", SCB->DFSR);
    rom_printf("    AFSR = 0x%08X\r\n", SCB->AFSR);

    __ASM volatile("BKPT #01");

    while(1);
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    __asm(/*"PUBLIC Hard_Fault_Handler\n"*/
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r0, MSP\n"
    "MRSNE r0, PSP\n"
    "B HardFault_Handler_c");
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
