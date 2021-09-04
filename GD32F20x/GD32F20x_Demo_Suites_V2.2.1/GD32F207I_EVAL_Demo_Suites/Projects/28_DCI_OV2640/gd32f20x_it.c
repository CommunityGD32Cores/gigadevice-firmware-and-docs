/*!
    \file  gd32f20x_it.c
    \brief interrupt service routines

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

#include "gd32f20x_it.h"
#include "systick.h"
#include "dci_ov2640.h"
#include "picture.h"
#include "gd32f207i_eval.h"

extern uint8_t capture_image[];
extern void lcd_config(void);
extern void image_save(void);
extern void image_display(uint32_t dispaly_image_addr);

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
    \brief      this function handles DCI interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DCI_IRQHandler(void)
{
    /* convert 320*240 size image to 240*272 size image */
    if(RESET != dci_interrupt_flag_get(DCI_INT_EF)){
        int i=0,x=0,y=0;  
        for(x=0;x<320;x++){
            for(y=0;y<240;y++){
                if(x<272){    
                    *(uint16_t *)(0xC0040000+2*i)=*(uint16_t *)(capture_image+2*((320*y) + x));
                    i++;
                }
            }
        } 
        dci_interrupt_flag_clear(DCI_INT_EF);
        dma_channel_enable(DMA1, DMA_CH5);
    }
}

/*!
    \brief      this function handles EXTI line0 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    /* press the "Wakeup" key, enter the interrupt */
    if(RESET != exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE)){
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
        
        /* enable "User" key interrupt */
        exti_interrupt_enable(USER_KEY_EXTI_LINE);
        
        /* disable layer 0 and layer 1 */
        tli_layer_disable(LAYER0);
        tli_layer_disable(LAYER1);
        tli_reload_config(TLI_REQUEST_RELOAD_EN);

        dci_ov2640_init();
        dma_channel_enable(DMA1, DMA_CH5);
        delay_1ms(100);
        /* enable DCI interface and start image capture */
        dci_enable();  
        dci_capture_enable();
        /* reconfigure the LCD and enable TLI */
        lcd_config();
        tli_layer_enable(LAYER0);
        tli_layer_enable(LAYER1);
        tli_reload_config(TLI_REQUEST_RELOAD_EN);   
        tli_enable();
    }
}

/*!
    \brief      this function handles EXTI line10 to EXTI line15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(USER_KEY_EXTI_LINE)){
        /* press the "User" key, enter the interrupt, and save photo */
        exti_interrupt_flag_clear(USER_KEY_EXTI_LINE);
        tli_layer_disable(LAYER0);
        tli_layer_disable(LAYER1);
        tli_reload_config(TLI_REQUEST_RELOAD_EN);
        tli_enable();
        
        /* save and display the photo */
        image_save();
        image_display((uint32_t)image_background1);
    }else if(RESET != exti_interrupt_flag_get(TAMPER_KEY_EXTI_LINE)){
        /* press the "Tamper" key, enter the interrupt, and display photo */
        exti_interrupt_flag_clear(TAMPER_KEY_EXTI_LINE);
        image_display(0xC0080000);
        /* disable "User" key */
        exti_interrupt_disable(USER_KEY_EXTI_LINE);
    }
}
