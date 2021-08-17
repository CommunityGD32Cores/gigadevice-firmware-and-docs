/*!
    \file    main.c
    \brief   UASRT receiver timeout

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

uint8_t rxbuffer[64];
uint8_t txbuffer[64];
extern __IO uint8_t txcount;
extern __IO uint16_t rxcount;
void nvic_config(void);
void usart5_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i = 0, j = 0;
    nvic_config();
    usart5_config();
    printf("a usart receive timeout test example!");

    while(1){
        if(0 == rxcount){
            /* enable the USART receive interrupt */
            usart5_interrupt_enable(USART5, USART5_INT_RBNE);
        }else{
            /* enable the USART receive timeout and configure the time of timeout */
            usart_receiver_timeout_enable(USART5);
            usart_receiver_timeout_threshold_config(USART5, 115200*3);

            while(RESET == usart5_flag_get(USART5, USART5_FLAG_RT));
            for(i=0; i<rxcount; i++){
                txbuffer[i] = rxbuffer[j++];
            }
            /* disable the USART receive interrupt and enable the USART transmit interrupt */
            usart5_interrupt_disable(USART5, USART5_INT_RBNE);
            usart5_interrupt_enable(USART5, USART5_INT_TBE);
            while(txcount < rxcount);
            usart5_flag_clear(USART5, USART5_FLAG_RT);
            txcount = 0;
            rxcount = 0;
            i = 0;
            j = 0;
        }
    }
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

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(USART5_IRQn, 0, 1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART5, (uint8_t)ch);
    while(RESET == usart5_flag_get(USART5, USART5_FLAG_TBE));
    return ch;
}
