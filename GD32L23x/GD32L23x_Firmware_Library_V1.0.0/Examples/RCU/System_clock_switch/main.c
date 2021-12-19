/*!
    \file    main.c
    \brief   system clock switch example

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

#include "gd32l23x.h"
#include <stdio.h>
#include "gd32l233r_eval.h"

static void _delay(uint32_t timeout);
static void switch_system_clock_to_36m_hxtal(void);
static void switch_system_clock_to_64m_irc16m(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);
    printf("\r\nCK_SYS switch test demo\r\n");

    /* disable the USART */
    usart_disable(EVAL_COM);

    /* switch system clock to 36MHz by HXTAL */
    switch_system_clock_to_36m_hxtal();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    _delay(1000);

    /* switch system clock to 64MHz by IRC16M */
    switch_system_clock_to_64m_irc16m();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    while(1) {
    }
}

/*!
    \brief      delay function
    \param[in]  timeout: time out
    \param[out] none
    \retval     none
*/
static void _delay(uint32_t timeout)
{
    __IO uint32_t i, j;
    for(i = 0; i < timeout; i++) {
        for(j = 0; j < 500; j++) {
        }
    }
}

/*!
    \brief      switch system clock to 36M by HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_36m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* select IRC16M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC16M);
    rcu_deinit();
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while(1) {
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT(1);

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* PLL = HXTAL / 2 * 9 = 36 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLMF | RCU_CFG0_PLLMF_6);
    RCU_CFG1 &= ~RCU_CFG1_PREDV;
    RCU_CFG0 |=  RCU_PLL_MUL9;
    RCU_CFG1 |= RCU_PLL_PREDV2;

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLL)) {
    }
}

/*!
    \brief      switch system clock to 180M by IRC16M
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_64m_irc16m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* select IRC16M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC16M);
    rcu_deinit();

    /* enable IRC16M */
    RCU_CTL |= RCU_CTL_IRC16MEN;

    /* wait until IRC16M is stable or the startup time is longer than IRC16M_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC16MSTB);
    } while((0U == stab_flag) && (IRC16M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_IRC16MSTB)) {
        while(1) {
        }
    }

    /* set the wait state counter value */
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | FMC_WAIT_STATE_1;

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* PLL = (IRC16M/2) * 8 = 64 MHz */
    RCU_CFG1 &= ~(RCU_CFG1_PREDV);
    RCU_CFG1 |= RCU_PLL_PREDV2;
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF);
    RCU_CFG0 |= (RCU_PLLSRC_IRC16M | RCU_PLL_MUL8);

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLL)) {
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}
