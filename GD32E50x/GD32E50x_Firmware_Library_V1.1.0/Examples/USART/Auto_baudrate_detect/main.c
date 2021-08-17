/*!
    \file    main.c
    \brief   USART5 Auto baudrate detection

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include <stdio.h>
#include "gd32e507z_eval.h"

void usart5_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{ 
    gd_eval_led_init(LED1);

    /* USART5 configuration */
    usart5_config();

    printf("\n\rUSART auto baudrate detection example\n\r");
    /* configure the auto-baud rate method */
    usart5_autobaud_detection_mode_config(USART5, USART5_ABDM_FTOF);
    /* enable autobaudrate feature */
    usart5_autobaud_detection_enable(USART5);

    /* wait until receive enable acknowledge flag is set */
    while(RESET == usart5_flag_get(USART5, USART5_FLAG_REA));

    /* wait until transmit enable acknowledge flag is set */
    while(RESET == usart5_flag_get(USART5, USART5_FLAG_TEA));

    /* loop until the end of autobaudrate phase */
    while(RESET == usart5_flag_get(USART5, USART5_FLAG_ABD));

    /* if autobaudbate error occurred */
    if(RESET == usart5_flag_get(USART5, USART5_FLAG_ABDE)){
        /* wait until RBNE flag is set */
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_RBNE));

        /* wait until TBE flag is set */    
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_TBE));

        /* USART auto baud rate detection finshed successfully */
        gd_eval_led_on(LED1);

        /* check the transfer complete flag */
        while (RESET == usart5_flag_get(USART5, USART5_FLAG_TC));

        usart_transmit_config(USART5, USART_TRANSMIT_DISABLE);
        usart_receive_config(USART5, USART_RECEIVE_DISABLE);
    }

    /* USART disable */
    usart_disable(USART5);
    while(1);
}

/*!
    \brief      configure USART5
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart5_config(void)
{
    /* enable USART5 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable AF clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable USART5 clock */
    rcu_periph_clock_enable(RCU_USART5);

    /* connect port to USARTx_Tx */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* configure USART5 Tx Rx as alternate function */
    gpio_afio_port_config(AFIO_PC6_USART5_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC7_USART5_CFG, ENABLE);

    /* USART configure */
    usart_deinit(USART5);
    usart_baudrate_set(USART5, 115200U);
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);

    usart_enable(USART5);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART5, (uint8_t) ch);
    while(RESET == usart5_flag_get(USART5, USART5_FLAG_TBE));
    return ch;
}
