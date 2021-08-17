/*!
    \file    main.c
    \brief   transfer data from FLASH to RAM

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h"
#include <stdio.h>
#include "gd32f1x0r_eval.h"

#define ACCESSSUM                  (1024U)
#define FMC_PAGE_SIZE              ((uint16_t)0x400)
#define WRITE_START_ADDR           ((uint32_t)0x08004000)
#define WRITE_END_ADDR             ((uint32_t)0x08004400)

ErrStatus fmc_erase_program(void);
void dma_config(void);
void led_config(void);

uint32_t destdata[256];
uint32_t *ptrd;
uint32_t transdata = 0x3210ABCD;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i;

    /* configure LED */
    led_config();

    while(ERROR == fmc_erase_program());

    /* configure DMA */
    dma_config();

    while(RESET != dma_flag_get(DMA_CH0, DMA_FLAG_FTF)){
        dma_flag_clear(DMA_CH0, DMA_FLAG_G);
        dma_flag_clear(DMA_CH0, DMA_FLAG_FTF);
        dma_flag_clear(DMA_CH0, DMA_FLAG_HTF);
    }

    /* compare destdata with transdata */
    for(i = 0; i < (ACCESSSUM / 4); i++){
        if(transdata != destdata[i])
        {
            gd_eval_led_off(LED3);
            gd_eval_led_off(LED4);
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
            break;
        }
    }

    while(1);
}
  
/*!
    \brief      configure LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init (LED1);
    gd_eval_led_init (LED2);
    gd_eval_led_init (LED3);
    gd_eval_led_init (LED4);

    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_on(LED3);
    gd_eval_led_on(LED4);
}

/*!
    \brief      configure LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* DMA channel0 initialize */
    dma_deinit(DMA_CH0);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)&destdata[0];
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_init_struct.number = ACCESSSUM / 4;
    dma_init_struct.periph_addr = (uint32_t)WRITE_START_ADDR;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init(DMA_CH0, &dma_init_struct);
    /* DMA channel0 mode configuration */
    dma_circulation_disable(DMA_CH0);
    dma_memory_to_memory_enable(DMA_CH0);
    /* enable DMA transfer */
    dma_channel_enable(DMA_CH0);
}

/*!
    \brief      erase and program the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
ErrStatus fmc_erase_program(void)
{
    uint32_t i, page_num;
    uint32_t count, address;
    fmc_state_enum fmcstatus;

    /* unlock the flash */
    fmc_unlock();

    /* define the number of page to be erased */
    page_num = (WRITE_END_ADDR - WRITE_START_ADDR) / FMC_PAGE_SIZE;

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_END);
    /* erase the flash pages */
    for(count = 0; count < page_num; count++){
        fmcstatus = fmc_page_erase(WRITE_START_ADDR + (FMC_PAGE_SIZE * count));
        if(FMC_READY != fmcstatus){
            return ERROR;
        }
        fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_END);
    }

    ptrd = (uint32_t*)WRITE_START_ADDR;
    for(i = 0; i < ACCESSSUM; i++){
        if(0xFFFFFFFF != *ptrd){ 
            return ERROR;
        }
        ptrd++;
    }

    /* program flash bank1 */
    address = WRITE_START_ADDR;
    while(address < WRITE_END_ADDR){
        fmcstatus = fmc_word_program(address, transdata);
        address = address + 4;
        if(FMC_READY != fmcstatus){
            return ERROR;
        }
        fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_END);
    }

    /* lock the flash */
    fmc_lock();

    return SUCCESS;
}
