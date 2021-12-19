/*!
    \file    gd32w515_it.c
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
#include "main.h"
#include "systick.h"

extern FlagStatus ch0_complete;
extern void *nsfunc;

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
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
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

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

/*!
    \brief      this function handles DMA1_Channel0_IRQHandler interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA1_Channel0_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA1, DMA_CH0, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA1, DMA_CH0, DMA_INT_FLAG_FTF);
        dma_interrupt_flag_clear(DMA1, DMA_CH0, DMA_INT_FLAG_HTF);
        ch0_complete = SET;
    }
}

/*!
    \brief      this function handles DMA1_Channel1_IRQHandler interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA1_Channel1_IRQHandler(void)
{
    ns_fptr callback_NS;
    if(RESET != dma_interrupt_flag_get(DMA1, DMA_CH1, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA1, DMA_CH1, DMA_INT_FLAG_FTF);
        dma_interrupt_flag_clear(DMA1, DMA_CH1, DMA_INT_FLAG_HTF);
        if(nsfunc != (void *)NULL){
            /* return function pointer with cleared LSB */
            callback_NS = (ns_fptr)cmse_nsfptr_create(nsfunc);
            callback_NS();
        }
    }
}

/*!
    \brief      this function handles DMA1_Channel2_IRQHandler interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA1_Channel2_IRQHandler(void)
{
    ns_fptr callback_NS;
    if(RESET != dma_interrupt_flag_get(DMA1, DMA_CH2, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA1, DMA_CH2, DMA_INT_FLAG_FTF);
        dma_interrupt_flag_clear(DMA1, DMA_CH2, DMA_INT_FLAG_HTF);
        if(nsfunc != (void *)NULL){
            /* return function pointer with cleared LSB */
            callback_NS = (ns_fptr)cmse_nsfptr_create(nsfunc);
            callback_NS();
        }
    }
}
