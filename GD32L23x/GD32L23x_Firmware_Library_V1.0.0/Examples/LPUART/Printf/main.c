/*!
    \file    main.c
    \brief   LPUART printf

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

void com_lpuart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    com_lpuart_init();
    printf("lpuart printf test!\r\n");

    while(1) {
    }
}

/*!
    \brief      configure LPUART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_lpuart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable LPUART clock */
    rcu_periph_clock_enable(RCU_LPUART);

    /* connect port to LPUART TX */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_2);
    /* connect port to LPUART RX */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_3);

    /* configure LPUART TX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

    /* configure LPUART RX as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);

    /* LPUART configure */
    lpuart_deinit();
    lpuart_word_length_set(LPUART_WL_8BIT);
    lpuart_stop_bit_set(LPUART_STB_1BIT);
    lpuart_parity_config(LPUART_PM_NONE);
    lpuart_baudrate_set(115200U);
    lpuart_receive_config(LPUART_RECEIVE_ENABLE);
    lpuart_transmit_config(LPUART_TRANSMIT_ENABLE);

    lpuart_enable();
}

/* retarget the C library printf function to the LPUART */
int fputc(int ch, FILE *f)
{
    lpuart_data_transmit((uint8_t)ch);
    while(RESET == lpuart_flag_get(LPUART_FLAG_TBE));

    return ch;
}
