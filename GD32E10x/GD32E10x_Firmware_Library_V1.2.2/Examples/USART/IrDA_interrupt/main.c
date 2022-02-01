/*!
    \file    main.c
    \brief   USART IrDA transmitter and receiver
    
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
    \version 2020-09-30, V1.1.0, firmware for GD32E10x
    \version 2020-12-31, V1.2.0, firmware for GD32E10x
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

#include "gd32e10x.h"
#include <stdio.h>
#include "gd32e10x_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE            (ARRAYNUM(txbuffer) - 1)

uint8_t txbuffer[] = "\n\rUSART IrDA interrupt test\n\r";
uint8_t tx_size = TRANSMIT_SIZE;
uint8_t rxbuffer[TRANSMIT_SIZE];
uint8_t rx_size = TRANSMIT_SIZE;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;
ErrStatus state = ERROR;

void usart_nvic_config(void);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    
    /* enable USART, GPIOA and GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_USART2);
    rcu_periph_clock_enable(RCU_AF);
    usart_nvic_config();
    
    /* connect port to USART1_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    /* connect port to USART1_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* connect port to USART2_Tx */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* connect port to USART2_Rx  */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    
    /* USART1 and USART2 baudrate configuration */
    usart_baudrate_set(USART1, 9600);
    usart_baudrate_set(USART2, 9600);
    
    /* configure USART word length */
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_word_length_set(USART2, USART_WL_8BIT);
    
    /* configure USART stop bits */
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    
    /* configure USART transmitter */
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    
    /* configure USART receiver */
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    
    /* enable USART */
    usart_enable(USART1);
    usart_enable(USART2);
    
    /* set the USART prescaler */
    usart_prescaler_config(USART1, 0x1);
    usart_prescaler_config(USART2, 0x1);
    
    /* set the USART IrDA mode */
    usart_irda_lowpower_config(USART1, USART_IRLP_NORMAL);
    usart_irda_lowpower_config(USART2, USART_IRLP_NORMAL);
    
    /* enable IrDA mode */
    usart_irda_mode_enable(USART1);
    usart_irda_mode_enable(USART2);
    
    /* enable the USART interrupt */    
    usart_interrupt_enable(USART2, USART_INT_RBNE);
    usart_interrupt_enable(USART1, USART_INT_TBE);
    
    while(rxcount != rx_size);
    /* if the transfer is successfully completed, LED2 and LED3 are on */
    state = memory_compare(txbuffer, rxbuffer, TRANSMIT_SIZE);
    if(SUCCESS == state){
        /* if success, LED2 and LED3 are on */
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
    }else{
        /* if failed, LED4 and LED5 are on */
        gd_eval_led_on(LED4);
        gd_eval_led_on(LED5); 
    } 
    while(1){
    }
}

/*!
    \brief      cofigure the NVIC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(USART2_IRQn, 0, 0);
    nvic_irq_enable(USART1_IRQn, 0, 1);
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
    while(length--){
        if(*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}
