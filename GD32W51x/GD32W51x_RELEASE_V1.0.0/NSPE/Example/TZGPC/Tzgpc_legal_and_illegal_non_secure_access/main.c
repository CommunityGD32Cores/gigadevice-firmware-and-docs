/*!
    \file    main.c
    \brief   TZGPC legal/illegal non secure access, secure code with TZEN = 1

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

#define TRNG_CTL_ADDR_S     TRNG_BASE_S         /* TRNG control register secure address */
#define TRNG_CTL_ADDR_NS    TRNG_BASE_NS        /* TRNG control register non-secure address */

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t temp;

    gd_eval_com_init(EVAL_COM0);
    rcu_periph_clock_enable(RCU_TRNG);
    rcu_periph_clock_enable(RCU_TZPCU);

    /* enable TRNG illegal access interrupt */
    tzgpc_tziac_interrupt_enable(TZGPC_PERIPH_TRNG);
    /* enable and set TZIAC_S_IRQn interrupt to the lowest priority */
    nvic_irq_enable(TZIAC_S_IRQn, 2U, 0U);

    /* configure TRNG to secure */
    tzgpc_tzspc_peripheral_attributes_config(TZGPC_PERIPH_TRNG, TZGPC_SEC);
    printf("TRNG is set to secure access only. \r\n");
    printf("trng control register reset value: 0x%08x\r\n\r\n", TRNG_CTL);
    printf("==========TZGPC legal/illegal non secure access example=========\n");

    /* legal secure write access to trng register when trng is secure */
    *(uint32_t*)TRNG_CTL_ADDR_S |= TRNG_CTL_TRNGEN;
    printf("legal secure write access complete, set TRNG_CTL=0x00000004. \r\n\r\n");

    /* legal secure read access to trng register when trng is secure */
    temp = *(uint32_t*)TRNG_CTL_ADDR_S;
    printf("legal secure read trng value: 0x%08x\n", temp);

    /* illegal non-secure write access to trng register when trng is secure */
    *(uint32_t*)TRNG_CTL_ADDR_NS &= ~TRNG_CTL_TRNGEN;
    printf("illegal non-secure write access complete, set TRNG_CTL=0x00000000. \r\n");
    printf("legal read trng control register value: 0x%08x\r\n\r\n", TRNG_CTL);

    /* illegal non-sercure read access to trng register when trng is secure */
    temp = *(uint32_t*)TRNG_CTL_ADDR_NS;
    printf("illegal read trng control value: 0x%08x\r\n", temp);

    while(1){
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}