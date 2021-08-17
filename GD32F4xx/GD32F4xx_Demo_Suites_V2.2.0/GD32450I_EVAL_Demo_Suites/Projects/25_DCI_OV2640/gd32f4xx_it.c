/*!
    \file    gd32f4xx_it.c
    \brief   interrupt service routines
    
    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx_it.h"
#include "systick.h"
#include "main.h"
#include "dci_ov2640.h"
#include "picture.h"

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
    while (1){
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
    while (1){
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
    while (1){
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
    while (1){
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
    \brief      this function handles DMA1_Channel7_IRQ interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA1_Channel7_IRQHandler(void)
{
    /* 320*240 size image convert to 240*272 size image */ 
    if(dma_interrupt_flag_get(DMA1,DMA_CH7,DMA_INTF_FTFIF))
    {         
        int i=0,x=0,y=0;  
        dma_channel_disable(DMA1, DMA_CH7);

        for(x=0;x<320;x++)
        {
            for(y=0;y<240;y++)
            {
                if(x<272)
                {    
                    *(uint16_t *)(0XC0400000+2*i)=*(uint16_t *)(0XC0000000+2*((320*y) + x));
                    i++;
                }
            }
        } 
        dma_interrupt_flag_clear(DMA1,DMA_CH7,DMA_INTC_FTFIFC);
        dma_channel_enable(DMA1, DMA_CH7);

    }       
}

/*!
    \brief      this function handles EXTI10_15_IRQ interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    /* press the "user" key, enter the interrupt, and save photo */
    if(exti_interrupt_flag_get(EXTI_14) != RESET)
    {        

        /* display the photo */  
        image_save();
        
        image_display((uint32_t)image_background1);
        /* clear the interrupt flag bit */
        exti_interrupt_flag_clear(EXTI_14);
    }
    /* press the "tamper" key, enter the interrupt, and display photo */
    if(exti_interrupt_flag_get(EXTI_13) != RESET)
    {   
        delay_1ms(100);

        image_display((uint32_t)0XC0800000); 
        /* disable "user" key */
        EXTI_RTEN &= ~EXTI_14;
        /* clear the interrupt flag bit */
        exti_interrupt_flag_clear(EXTI_13);
    }
}

/*!
    \brief      this function handles EXTI0_IRQ interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    /* press the "wakeup" key, enter the interrupt */
    if(exti_interrupt_flag_get(EXTI_0) != RESET)
    {

        
        /* enable "user" key */
        EXTI_PD = EXTI_14;
        EXTI_RTEN |= EXTI_14;
                
        tli_layer_disable(LAYER0);
        tli_layer_disable(LAYER1);
        /* reload configuration */
        tli_reload_config(TLI_REQUEST_RELOAD_EN);
        delay_1ms(100);  
        
        dci_ov2640_init();  
        dma_interrupt_enable(DMA1, DMA_CH7,DMA_CHXCTL_FTFIE);
        dma_channel_enable(DMA1, DMA_CH7);  
        delay_1ms(10);    
        /* enable DCI interface */
        dci_enable(); 
        /* start image capture */ 
        dci_capture_enable();
        delay_1ms(100);
        lcd_config();
        /* enable layer0 */
        tli_layer_enable(LAYER0);
        /* enable layer1 */
        tli_layer_enable(LAYER1);
        /* reload configuration */
        tli_reload_config(TLI_REQUEST_RELOAD_EN);
        /* enable tli */
        tli_enable(); 
        /* clear the interrupt flag bit */
        exti_interrupt_flag_clear(EXTI_0);
    }
}
