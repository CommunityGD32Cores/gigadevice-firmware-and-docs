/*!
    \file    main.c
    \brief   ICACHE test and memory map from RAM to RAM

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

#define BUFFER_SIZE     (1024U)
uint8_t data_Buffer_1[BUFFER_SIZE];
uint8_t data_Buffer_2[BUFFER_SIZE];

extern ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    __IO uint8_t *mem_addr;
    uint32_t index;
    icache_remap_struct ICACHE_struct;

    /* enable the LED GPIO clock */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    /* disable icache pin */
    icache_disable();
    /* enable icache hit miss monitor */
    icache_monitor_enable(ICACHE_MONITOR_HIT_MISS);
    /* set icache memory remap parameters */
    ICACHE_struct.region_num = 0U;
    ICACHE_struct.base_address = 0x10000000UL;
    ICACHE_struct.remap_address = 0x30000000UL;
    ICACHE_struct.remap_size = ICACHE_REMAP_2M;
    ICACHE_struct.burst_type = ICACHE_WRAP_BURST;
    /* enable icache remap function */
    icache_remap_enable(&ICACHE_struct);
    /* enable icache */
    icache_enable();
    /* get data from remap address */
    mem_addr = (uint8_t *)(0x30000000UL);
    for (index = 0; index < BUFFER_SIZE; index++){
        data_Buffer_1[index] = *mem_addr;
        mem_addr++;
    }
		
    /* get data from base address */
    mem_addr = (uint8_t *)(0x10000000UL);
    for (index = 0; index < BUFFER_SIZE; index++){
        data_Buffer_2[index] = *mem_addr;
        mem_addr++;
    }
    /* check data */
    if (ERROR == memory_compare(data_Buffer_1, data_Buffer_2, BUFFER_SIZE)){
        gd_eval_led_on(LED2);
        while (1){
        }
    }
    /* disable icache function */
    icache_disable();
    while(1){
        gd_eval_led_on(LED3);
    }
}


/*!
\brief      memory compare function
\param[in]  src : source data
\param[in]  dst : destination data
\param[in]  length : the compare data length
\param[out] none
\retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length)
{
    uint32_t count = 0;
    while (length--){
        if (*src++ != *dst++)
            return ERROR;
        else{
            count++;
        }
    }
    return SUCCESS;
}