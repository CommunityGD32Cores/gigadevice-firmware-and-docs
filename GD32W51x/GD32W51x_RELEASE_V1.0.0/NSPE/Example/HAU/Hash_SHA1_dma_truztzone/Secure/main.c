/*!
    \file    main.c
    \brief   HASH SHA1 DMA trustzone, secure code with TZEN = 1
    
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

#include "gd32w51x.h"
#include "gd32w515p_eval.h"
#include <stdio.h>
#include "main.h"
#include <arm_cmse.h>

#define NONSECURE_START  ((uint32_t)0x08040000U)       /* non-secure start address */

void *f_secure_fault = NULL;
void *f_secure_error = NULL;

void tzbmpc_config();
void nonsecure_init();
void nvic_config(void);
void rcu_config(void);
CMSE_NS_ENTRY void hash_sha1_dma(uint8_t *input, uint32_t in_length, uint8_t output[20]);
CMSE_NS_ENTRY void callback_config(void *fault, void *error);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure clocks */
    rcu_config();
    
    /* configure SRAM1 to non-secure */
    tzbmpc_config();    
    /* configure LED1, LED2, LED3 to non-secure */
    gpio_bit_reset_sec_cfg(GPIOA, GPIO_PIN_15);
    gpio_bit_reset_sec_cfg(GPIOB, GPIO_PIN_6);
    gpio_bit_reset_sec_cfg(GPIOB, GPIO_PIN_8);
    
    /* configure NVIC */
    nvic_config();

    /* setup and jump to non-secure */
    nonsecure_init();

    while(1){
    }
}

/*!
    \brief      configure NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    /* enable illegal access interrupt */
    tzpcu_tziac_interrupt_enable(TZPCU_PERIPH_HAU);
    tzpcu_tziac_interrupt_enable(TZPCU_PERIPH_SRAM1);
    tzpcu_tziac_interrupt_enable(TZPCU_PERIPH_TZBMPC1_REG);
    tzpcu_tziac_interrupt_enable(TZPCU_PERIPH_TZSPC);
    tzpcu_tziac_interrupt_enable(TZPCU_PERIPH_TZIAC);
    
    /* clear all illegal access flags */
    tzpcu_tziac_flag_clear(TZPCU_PERIPH_ALL);
    
    /* configure priority group */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE3_SUB1);
    
    nvic_irq_enable(TZIAC_S_IRQn, 0, 0);
    
    /* enable SecureFault handler (HardFault is default) */
    SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk; 
}    

/*!
    \brief      configure clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);
    /* enable SRAM1 clock */
    rcu_periph_clock_enable(RCU_SRAM1);
    /* enable DMA1 clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable HAU clock */
    rcu_periph_clock_enable(RCU_HAU); 
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
}

/*!
    \brief      configure tzbmpc
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tzbmpc_config(void)
{
    uint16_t block_number = 0U;
    /* enable TZPCU clock */
    rcu_periph_clock_enable(RCU_TZPCU);

    /* SRAM1 is used to nonsecure code, so all blocks of SRAM1 should set to nonsecure */
    for(block_number = 0U; block_number <= TZBMPC1_BLOCK_NUMBER; block_number++){
        tzpcu_tzbmpc_block_secure_access_mode_config(TZBMPC1, block_number, BLOCK_SECURE_ACCESS_MODE_NSEC);
    }
}

/*!
    \brief      setup and jump to non-secure
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nonsecure_init(void)
{
    uint32_t NonSecure_StackPointer = (*((uint32_t *)(NONSECURE_START)));
    funcptr_NS NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)(NONSECURE_START + 4U)));
    /* Set non-secure vector table location */
    SCB_NS->VTOR = NONSECURE_START;
    /* Set non-secure stack pointer */
    __TZ_set_MSP_NS(NonSecure_StackPointer);
    /* Start non-secure application */
    NonSecure_ResetHandler();
}

/*!
    \brief      calculate digest using SHA1 in HASH mode
    \param[in]  input: pointer to the input buffer     
    \param[in]  in_length: length of the input buffer  
    \param[out] output: the result digest    
    \retval     none
*/
void hash_sha1_dma(uint8_t *input, uint32_t in_length, uint8_t output[20])
{
    dma_multi_data_parameter_struct dma_init_parameter;
    hau_init_parameter_struct hau_initpara;
    hau_digest_parameter_struct digest_para;
    __IO uint16_t num_last_valid = 0;
    __IO uint32_t meg_burst = 0;
    uint32_t outputaddr = (uint32_t)output;

    /* condigure HAU attributes to secure, non-priviledge */
    tzpcu_tzspc_peripheral_attributes_config(TZPCU_PERIPH_HAU, TZPCU_SEC | TZPCU_NPRIV);
    /* condigure DMA1 channel attributes */
    dma_security_config(DMA1, DMA_CH7, DMA_CHANNEL_SECURE | DMA_CHANNEL_SOURCE_NONSECURE | DMA_CHANNEL_DESTINATION_SECURE);
        
    /* HAU configuration */  
    hau_deinit();
    hau_init_struct_para_init(&hau_initpara);
    hau_initpara.algo = HAU_ALGO_SHA1;
    hau_initpara.mode = HAU_MODE_HASH;
    hau_initpara.datatype = HAU_SWAPPING_8BIT;
    hau_init(&hau_initpara);

    if(in_length % 4){
        meg_burst++;
    }
    meg_burst += (in_length/4);
    num_last_valid = 8 * (in_length % 4);
    /* configure the number of valid bits in last word of the message */
    hau_last_word_validbits_num_config(num_last_valid);
      
    /* DMA peripheral configure */
    dma_deinit(DMA1, DMA_CH7);
    dma_multi_data_para_struct_init(&dma_init_parameter);
    dma_init_parameter.periph_addr = (uint32_t)(&HAU_DI);
    dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_parameter.memory0_addr = (uint32_t)input;
    dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.periph_burst_width = DMA_MEMORY_BURST_SINGLE;
    dma_init_parameter.critical_value = DMA_FIFO_4_WORD;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_parameter.number = meg_burst;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_multi_data_mode_init(DMA1,DMA_CH7, &dma_init_parameter);

    dma_channel_subperipheral_select(DMA1, DMA_CH7, DMA_SUBPERI2);
    
    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH7);
    
    hau_dma_enable();
    /* wait until the last transfer from OUT FIFO */
    while(!dma_flag_get(DMA1, DMA_CH7, DMA_FLAG_FTF)){
    }

    /* wait until the busy flag is reset */
    while(hau_flag_get(HAU_FLAG_BUSY)){
    }

    hau_digest_read(&digest_para);
    *(uint32_t*)(outputaddr)  = __REV(digest_para.out[0]);
    outputaddr += 4U;
    *(uint32_t*)(outputaddr)  = __REV(digest_para.out[1]);
    outputaddr += 4U;
    *(uint32_t*)(outputaddr)  = __REV(digest_para.out[2]);
    outputaddr += 4U;
    *(uint32_t*)(outputaddr)  = __REV(digest_para.out[3]);
    outputaddr += 4U;
    *(uint32_t*)(outputaddr)  = __REV(digest_para.out[4]);
}    

/*!
    \brief      callback function configuration
    \param[in]  fault: function pointer for SecureFault_Handler
    \param[in]  error: function pointer for TZIAC_S_IRQHandler
    \param[out] none
    \retval     none
*/
void callback_config(void *fault, void *error)
{
    if(fault != NULL){
        f_secure_fault = fault;
    }
    if(error != NULL){
        f_secure_error = error;
    }
}
