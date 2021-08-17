/*!
    \file    main.c
    \brief   USART transmit and receive interrupt

    \version 2020-12-09, V1.0.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include <stdio.h>
#include "gd32e507z_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE            (ARRAYNUM(txbuffer) - 1)

uint8_t txbuffer[] = "\n\rUSART interrupt test\n\r";
uint8_t rxbuffer[32];
uint8_t tx_size = TRANSMIT_SIZE;
uint8_t rx_size = 32;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;

void usart5_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* USART interrupt configuration */
    nvic_irq_enable(USART5_IRQn, 0, 0);
    /* configure USART5 */
    usart5_config();
    /* enable USART5 TBE interrupt */
    usart5_interrupt_enable(USART5, USART5_INT_TBE);

    /* wait until USART send the transmitter_buffer */
    while(txcount < tx_size);

    while(RESET == usart5_flag_get(USART5, USART5_FLAG_TC));

    usart5_interrupt_enable(USART5, USART5_INT_RBNE);

    /* wait until USART receive the receiver_buffer */
    while(rxcount < rx_size);
    if(rxcount == rx_size)
        printf("\n\rUSART receive successfully!\n\r");
    while (1);
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
    usart_data_transmit(USART5, (uint8_t)ch);
    while(RESET == usart5_flag_get(USART5, USART5_FLAG_TBE));
    return ch;
}
