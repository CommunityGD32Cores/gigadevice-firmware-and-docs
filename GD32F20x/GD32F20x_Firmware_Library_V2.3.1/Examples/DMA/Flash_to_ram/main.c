/*!
    \file    main.c
    \brief   transfer data from FLASH to RAM

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
    \version 2021-07-30, V2.3.0, firmware for GD32F20x
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

#include "gd32f20x.h"
#include <string.h>
#include "gd32f20x_eval.h"

#define TRANSFER_NUM                     0x400                     /* configuration value in bytes */
#define FMC_PAGE_SIZE                    ((uint16_t)0x800)
#define BANK0_WRITE_START_ADDR           ((uint32_t)0x08004000)

void rcu_config(void);
void nvic_config(void);
void led_config(void);
ErrStatus data_check(uint32_t *data_addr, uint32_t num, uint32_t data);
ErrStatus flash_erase(uint32_t addr, uint32_t size);
ErrStatus flash_write(uint32_t addr, uint32_t size, uint32_t data);
void dma_config(void);

__IO uint32_t g_dmacomplete_flag = 0;
uint8_t g_destbuf[TRANSFER_NUM];
const uint32_t transdata = 0x3210ABCD;
fmc_state_enum fmcstatus = FMC_READY;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* NVIC configuration */
    nvic_config();
    /* LED configuration */
    led_config();
    /* erase flash */
    if(ERROR == flash_erase(BANK0_WRITE_START_ADDR, TRANSFER_NUM)) {
        while(1);
    }
    /* check flash */
    if(ERROR == data_check((uint32_t *)BANK0_WRITE_START_ADDR, TRANSFER_NUM, 0xFFFFFFFF)) {
        while(1);
    }
    /* write to flash */
    if(ERROR == flash_write(BANK0_WRITE_START_ADDR, TRANSFER_NUM, transdata)) {
        while(1);
    }
    /* DMA configuration */
    dma_config();

    /* wait for DMA interrupt */
    while(0 == g_dmacomplete_flag);

    /* transfer success */
    if(ERROR != data_check((uint32_t *)g_destbuf, TRANSFER_NUM, transdata)) {
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED3);
    } else {
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED4);
    }

    while(1) {
    }
}

/*!
    \brief      check flash data with the expected data
    \param[in]  data_addr: the start address of flash to be checked
    \param[in]  num: data bytes to check
    \param[in]  data: the expected data
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus data_check(uint32_t *data_addr, uint32_t num, uint32_t data)
{
    ErrStatus ret = SUCCESS;
    uint32_t i, count;
    uint32_t *ptrd;

    if(NULL != data_addr) {
        ptrd = data_addr;
        count = num / sizeof(*ptrd);

        for(i = 0; i < count; i++) {
            if(data != *ptrd) {
                ret = ERROR;
                break;
            }
            ptrd++;
        }
    } else {
        ret = ERROR;
    }
    return ret;
}

/*!
    \brief      erase flash
    \param[in]  addr: the start address of flash to be erased
    \param[in]  size: data bytes to erase
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus flash_erase(uint32_t addr, uint32_t size)
{
    ErrStatus ret = SUCCESS;
    uint32_t i, count;
    uint32_t wperror = 0;

    /* unlock the flash bank1 program erase controller */
    fmc_unlock();

    /* define the number of page to be erased */
    count = size / FMC_PAGE_SIZE;
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END);

    /* erase the flash pages */
    for(i = 0; i <= count; i++) {
        fmcstatus = fmc_page_erase(addr + (FMC_PAGE_SIZE * i));
        wperror += (fmcstatus == FMC_WPERR);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END);
    }

    if(0 != wperror) {
        ret = ERROR;
    }

    /* unlock the flash bank1 program erase controller */
    fmc_lock();

    return ret;
}

/*!
    \brief      write to flash
    \param[in]  addr: the start address of flash to be written
    \param[in]  size: data bytes to write
    \param[in]  data: the value to be wriiten
    \param[out] none
    \retval     ErrStatus: ERROR or SUCCESS
*/
ErrStatus flash_write(uint32_t addr, uint32_t size, uint32_t data)
{
    ErrStatus ret = SUCCESS;
    uint32_t wperror = 0, count;

    /* unlock the flash bank1 program erase controller */
    fmc_unlock();
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END);

    /* program flash bank1 */
    wperror = 0;
    count = addr + size;

    while(addr < count) {
        fmcstatus = fmc_word_program(addr, data);
        addr = addr + 4;
        wperror += (FMC_WPERR == fmcstatus);
        fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END);
    }

    if(wperror != 0) {
        ret = ERROR;
    }

    fmc_lock();

    return ret;
}

/*!
    \brief      configure DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    memset(g_destbuf, 0, TRANSFER_NUM);
    /* DMA0 channel0 initialize */
    dma_deinit(DMA0, DMA_CH0);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)g_destbuf;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = TRANSFER_NUM;
    dma_init_struct.periph_addr = (uint32_t)BANK0_WRITE_START_ADDR;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_init_struct);
    /* DMA0 channel0 mode configuration */
    dma_circulation_disable(DMA0, DMA_CH0);
    dma_memory_to_memory_enable(DMA0, DMA_CH0);
    /* DMA0 channel0 interrupt configuration */
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
    /* enable DMA0 transfer */
    dma_channel_enable(DMA0, DMA_CH0);
}

/*!
    \brief      configure LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* LED off */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA0_Channel0_IRQn, 0, 0);
}
