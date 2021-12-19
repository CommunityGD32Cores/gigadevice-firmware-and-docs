/*!
    \file    main.c
    \brief   TZSPC legal/illegal privilege access

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
#include "main.h"

#define SVC(a)       __ASM volatile("SVC %[arg1]           \n" : :[arg1] "I" (a) )

/* get privilege level */
uint32_t privilege_level_get(void);
/* set privilege level */
void privilege_level_set(uint32_t privilege_level);

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

    /* configure TRNG to secure */
    tzgpc_tzspc_peripheral_attributes_config(TZGPC_PERIPH_TRNG, TZGPC_PRIV);
    printf("TRNG is set to privilege access only. \r\n");
    printf("trng control register reset value: 0x%08x\r\n\r\n", TRNG_CTL);

    printf("==========TZGPC legal/illegal privilege access example=========\n");

    /* change to privilege mode */
    privilege_level_set(PRIV_MODE);
    printf("now thread mode is changed to %s\n", (PRIV_MODE==privilege_level_get())?("privilege mode"):("non-privilege mode"));
    /* legal privilege write access to trng register when trng is privilege level */
    TRNG_CTL |= TRNG_CTL_TRNGEN;
    printf("legal privilege write access complete, set TRNG_CTL=0x00000004. \r\n\r\n");

    /* legal read access to trng register when trng is privilege level */
    temp = TRNG_CTL;
    printf("legal read trng control value: 0x%08x\r\n\r\n", temp);

    /* change to non-privilege mode */
    privilege_level_set(NPRIV_MODE);
    printf("now thread mode is changed to %s\r\n", (PRIV_MODE==privilege_level_get())?("privilege mode"):("non-privilege mode"));
    /* illegal non-privilege write access to trng register when trng is privilege level */
    TRNG_CTL &= ~TRNG_CTL_TRNGEN;
    printf("illegal non-privilege write access complete, set TRNG_CTL=0x00000000. \r\n");
    /* change to privilege mode */
    privilege_level_set(PRIV_MODE);
    printf("now thread mode is changed to %s\n", (PRIV_MODE==privilege_level_get())?("privilege mode"):("non-privilege mode"));
    printf("legal read trng control register value: 0x%08x\r\n\r\n", TRNG_CTL);

    /* change to non-privilege mode */
    privilege_level_set(NPRIV_MODE);
    printf("now thread mode is changed to %s\n", (PRIV_MODE==privilege_level_get())?("privilege mode"):("non-privilege mode"));
    /* legal read access to trng register when trng is privilege levele */
    temp = TRNG_CTL;
    printf("illegal read trng control value: 0x%08x\r\n", temp);

    while(1){
    }
}

/*!
    \brief      get privilege level
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint32_t privilege_level_get(void)
{
    uint32_t control_value = __get_CONTROL();
    return (PRIV_MODE==(control_value&0x01))?(PRIV_MODE):(NPRIV_MODE);
}

/*!
    \brief      set privilege level
    \param[in]  privilege_level: privilege level
                only one parameter can be selected which is shown as below:
      \arg        PRIV_MODE: privilege mode
      \arg        NPRIV_MODE: non-privilege mode
    \param[out] none
    \retval     none
*/
void privilege_level_set(uint32_t privilege_level)
{
    if(PRIV_MODE == privilege_level){
        SVC(PRIV_MODE);
    } else{
        SVC(NPRIV_MODE);
    }
} 

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}