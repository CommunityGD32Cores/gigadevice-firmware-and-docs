/*!
    \file    delay.c
    \brief   delay functions
    
    \version 2017-06-28, V1.0.0, demo for GD32F3x0
    \version 2019-06-01, V2.0.0, demo for GD32F3x0
    \version 2021-05-31, V2.1.0, demo for GD32F3x0
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

#include "gd32f3x0.h"
#include "delay.h"

static uint8_t  fac_us = 0;
static uint16_t fac_ms = 0;

/*!
    \brief      initialization time delay function
    \param[in]  sysclk: system clock
    \param[out] none
    \retval     none
*/
void delay_init(uint8_t sysclk)
{
    SysTick->CTRL &= 0xfffffffb;

    fac_us = sysclk / 8;

    fac_ms = (uint16_t)fac_us * 1000;
}

/*!
    \brief      delay to ms
    \param[in]  nms: specifie the delay time length
    \param[out] none
    \retval     none
*/
void delay_ms(uint16_t nms)
{
    uint32_t systick_clrl;

    /* load time */
    SysTick->LOAD = (uint32_t)nms * fac_ms;

    /* clear counter */
    SysTick->VAL = 0x00;

    SysTick->CTRL = 0x01;

    do{
        systick_clrl = SysTick->CTRL;
    }while(systick_clrl & 0x01 && !(systick_clrl & (1 << 16)));

    /* close the counter */
    SysTick->CTRL = 0x00;

    /* clear counter */
    SysTick->VAL = 0X00;
}

/*!
    \brief      delay to nus
    \param[in]  nus: specifie the delay time length
    \param[out] none
    \retval     none
*/
void delay_us(uint32_t nus)
{
    uint32_t systick_clrl;

    /* load time */
    SysTick->LOAD = nus * fac_us;

    /* clear counter */
    SysTick->VAL = 0x00;

    SysTick->CTRL = 0x01;

    do{
        systick_clrl = SysTick->CTRL;
    }while(systick_clrl & 0x01 && !(systick_clrl & (1 << 16)));

    /* close the counter */
    SysTick->CTRL = 0x00;

    /* clear counter */
    SysTick->VAL = 0X00;
}
