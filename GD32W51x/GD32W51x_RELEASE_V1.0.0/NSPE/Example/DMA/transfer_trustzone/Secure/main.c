/*!
    \file    main.c
    \brief   DMA transfer with security and non-security, secure code with TZEN = 1

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
#include "systick.h"
#include "gd32w515p_eval.h"
#include "main.h"

#define BUFFER_SIZE             32U
#define NONE                    0xffffffff
#define TZBMPC1_BLOCK_NUMBER    255                          /* TZBMPC1 block number */

#define NONSECURE_START        ((uint32_t)0x08040000U)       /* nonsecure start address */

const uint32_t buffer0[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};
uint32_t buffer1[BUFFER_SIZE] = {0};
uint32_t buffer2[BUFFER_SIZE] = {0};

FlagStatus ch0_complete = RESET;
FlagStatus ch1_complete = RESET;
FlagStatus ch2_complete = RESET;

void *nsfunc = NULL;

CMSE_NS_ENTRY void dma1_config(dma_channel_enum channelx, uint32_t address, void *func);
ErrStatus data_check(uint32_t *src, uint32_t *des);
CMSE_NS_ENTRY ErrStatus last_data_check(void);
void tzbmpc_config();
void nonsecure_init();

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable SecureFault handler */
    SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

    /* configure TZBMPC */
    tzbmpc_config();

    /* configure systick */
    systick_config();

    /* DMA1 CH0 configure */
    dma1_config(DMA_CH0, NONE, NULL);

    /* initilize the LED1 */
    gd_eval_led_init(LED1);
    /* configure LED3 as non-secure */
    gpio_bit_reset_sec_cfg(LED3_GPIO_PORT, LED3_PIN);

    /* wait until transfer complete */
    while(ch0_complete == RESET){
    }

    /* data check */
    if(ERROR != data_check((uint32_t *)buffer0, buffer1)){
        gd_eval_led_on(LED1);
    }

    /* setup and jump to non-secure */
    nonsecure_init();

    while(1){
    }
}

/*!
    \brief      configure tzbmpc
    \param[in]  none
    \param[out] none
    \retval     none
*/
void tzbmpc_config()
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
void nonsecure_init()
{
    uint32_t NonSecure_StackPointer = (*((uint32_t *)(NONSECURE_START)));
    ns_fptr nonsecure_reset_handler = (ns_fptr)(*((uint32_t *)(NONSECURE_START + 4U)));

    /* set non-secure vector table location */
    SCB_NS->VTOR = NONSECURE_START;
    /* set non-secure stack pointer */
    __TZ_set_MSP_NS(NonSecure_StackPointer);
    /* start non-secure application */
    nonsecure_reset_handler();
}

/*!
    \brief      configure the DMA1
    \param[in]  channelx: specify which DMA1 channel
      \arg        DMA_CHx(x=0..2)
    \param[in]  address: buffer address
    \param[in]  func: callback function
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY void dma1_config(dma_channel_enum channelx, uint32_t address, void *func)
{
    dma_single_data_parameter_struct dma_init_parameter;
    dma_single_data_para_struct_init(&dma_init_parameter);
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1, channelx);
    nsfunc = func;

    switch(channelx){
        case DMA_CH0:
            /* configure DMA1 channel0 */
            dma_init_parameter.periph_addr = (uint32_t)buffer0;
            dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
            dma_init_parameter.memory0_addr = (uint32_t)buffer1;
            dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
            dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
            dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
            dma_init_parameter.direction = DMA_MEMORY_TO_MEMORY;
            dma_init_parameter.number = BUFFER_SIZE;
            dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
            dma_single_data_mode_init(DMA1, channelx, &dma_init_parameter);

            /* configure security attribute */
            dma_security_config(DMA1, channelx, DMA_CHANNEL_SECURE | DMA_CHANNEL_SOURCE_SECURE | DMA_CHANNEL_DESTINATION_SECURE);

            /* configure DMA interrupt */
            nvic_irq_enable(DMA1_Channel0_IRQn, 0, 0);
            dma_interrupt_enable(DMA1, channelx, DMA_CHXCTL_FTFIE);

            /* enable DMA channel */
            dma_channel_enable(DMA1, channelx);
            break;
        case DMA_CH1:
            /* configure DMA1 channel1 */
            dma_init_parameter.periph_addr = (uint32_t)buffer1;
            dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
            dma_init_parameter.memory0_addr = address;
            dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
            dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
            dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
            dma_init_parameter.direction = DMA_MEMORY_TO_MEMORY;
            dma_init_parameter.number = BUFFER_SIZE;
            dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
            dma_single_data_mode_init(DMA1, channelx, &dma_init_parameter);

            /* configure security attribute */
            dma_security_config(DMA1, channelx, DMA_CHANNEL_SECURE | DMA_CHANNEL_SOURCE_SECURE | DMA_CHANNEL_DESTINATION_NONSECURE);

            /* configure DMA interrupt */
            nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);
            dma_interrupt_enable(DMA1, channelx, DMA_CHXCTL_FTFIE);

            /* enable DMA channel */
            dma_channel_enable(DMA1, channelx);
            break;
        case DMA_CH2:
            /* configure DMA1 channel2 */
            dma_init_parameter.periph_addr = address;
            dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
            dma_init_parameter.memory0_addr = (uint32_t)buffer2;
            dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
            dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
            dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
            dma_init_parameter.direction = DMA_MEMORY_TO_MEMORY;
            dma_init_parameter.number = BUFFER_SIZE;
            dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
            dma_single_data_mode_init(DMA1, channelx, &dma_init_parameter);

            /* configure security attribute */
            dma_security_config(DMA1, channelx, DMA_CHANNEL_SECURE | DMA_CHANNEL_SOURCE_NONSECURE | DMA_CHANNEL_DESTINATION_SECURE);

            /* configure DMA interrupt */
            nvic_irq_enable(DMA1_Channel2_IRQn, 0, 0);
            dma_interrupt_enable(DMA1, channelx, DMA_CHXCTL_FTFIE);

            /* enable DMA channel */
            dma_channel_enable(DMA1, channelx);
            break;
        default:
            while(1){
            }
    }
}

/*!
    \brief      check data
    \param[in]  none
    \param[out] none
    \retval     none
*/
ErrStatus data_check(uint32_t *src, uint32_t *des)
{
    uint32_t i = BUFFER_SIZE;

    while(i--){
        if(*src != *des){
            return ERROR;
        }
        src++;
        des++;
    }

    return SUCCESS;
}

/*!
    \brief      check data
    \param[in]  none
    \param[out] none
    \retval     none
*/
CMSE_NS_ENTRY ErrStatus last_data_check(void)
{
    uint32_t i = BUFFER_SIZE;
    uint32_t *src = (uint32_t *)buffer0;
    uint32_t *des = buffer2;

    while(i--){
        if(*src != *des){
            return ERROR;
        }
        src++;
        des++;
    }

    return SUCCESS;
}
