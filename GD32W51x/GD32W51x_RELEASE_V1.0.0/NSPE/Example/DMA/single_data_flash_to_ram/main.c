/*!
    \file    main.c
    \brief   DMA transfer data from FLASH to RAM 

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
#include <stdio.h>
#include "gd32w515p_eval.h"

#define TRANSFER_NUM                     (0x400)                     /* configuration value in words */
#define FMC_PAGE_SIZE                    ((uint16_t)0x1000)
#define FLASH_WRITE_START_ADDR           ((uint32_t)0x0800A000)

static void led_config(void);
static void fill_buffer_size(void);
static void fmc_write_32bit_data(uint32_t address, uint16_t length, int32_t* data_32);
static void dma_config(void);
static ErrStatus memory_compare(int32_t* src, int32_t* dst, uint16_t length);

int32_t source[TRANSFER_NUM];
int32_t destination[TRANSFER_NUM];
ErrStatus status;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{

    /* configure LED */
    led_config();

    /* fill the buffer with specified value */
    fill_buffer_size();

    /* write 32 bit length data to a given address */
    fmc_write_32bit_data(FLASH_WRITE_START_ADDR, TRANSFER_NUM, source);

    /* configure the DMA channel */
    dma_config();

    /* wait for DMA transfer to complete */
    while(!dma_flag_get(DMA1, DMA_CH0, DMA_FLAG_FTF));

    /* compare data */
    status = memory_compare(source, destination, TRANSFER_NUM);
    if(SUCCESS == status){
        gd_eval_led_on(LED1);
    }else{
        gd_eval_led_on(LED3);
    }

    while(1);
}

/*!
    \brief      configure LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void led_config(void)
{
    /* LED initialize */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED3);
}

/*!
    \brief      fill the buffer with specified value
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void fill_buffer_size(void)
{
    uint32_t i;
    for(i=0;i < TRANSFER_NUM;i++){
        source[i] = i;
    }
}

/*!
    \brief      write 32 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x080FFFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
static void fmc_write_32bit_data(uint32_t address, uint16_t length, int32_t* data_32)
{
    uint32_t count, i;
    uint32_t wperror = 0;
    fmc_state_enum fmcstatus = FMC_READY;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* define the number of page to be erased */
    count = (TRANSFER_NUM * 4) / FMC_PAGE_SIZE;
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);
    /* erase the flash pages */
    for(i = 0; i < count; i++){
        fmcstatus = fmc_page_erase(FLASH_WRITE_START_ADDR + (FMC_PAGE_SIZE * i));
        wperror += (fmcstatus == FMC_WPERR);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);
    }

    if(wperror != 0){
        while(1){
        }
    }

    /* write data_32 to the corresponding address */
    for(i=0; i<length; i++){
        if(FMC_READY == fmc_word_program(address, data_32[i])){
            address = address + 4;
        }else{ 
            while(1);
        }
    }

    /* lock the flash program erase controller */
    fmc_lock();
}

/*!
    \brief      configure the DMA channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void dma_config(void)
{
    dma_single_data_parameter_struct dma_init_parameter;
    dma_single_data_para_struct_init(&dma_init_parameter);

    /* peripheral clock enable */
    rcu_periph_clock_enable(RCU_DMA1);

    /* DMA peripheral configure */
    dma_deinit(DMA1,DMA_CH0);

    dma_init_parameter.periph_addr = (uint32_t)FLASH_WRITE_START_ADDR;
    dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_parameter.memory0_addr = (uint32_t)destination;
    dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
    dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_init_parameter.direction = DMA_MEMORY_TO_MEMORY;
    dma_init_parameter.number = TRANSFER_NUM;
    dma_init_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_init_parameter);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(int32_t* src, int32_t* dst, uint16_t length)
{
    while (length--){
        if (*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
