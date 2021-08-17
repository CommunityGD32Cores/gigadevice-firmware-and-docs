/*!
    \file    main.c
    \brief   TMU calculation demo

    \version 2020-06-30, V1.0.0, demo for GD32E50x
    \version 2020-08-26, V1.1.0, demo for GD32E50x
    \version 2021-03-31, V1.2.0, demo for GD32E50x
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
#include "tmu_math.h"

#define RXBUFFER   11
#define DATA_ERRO  0
#define DATA_OK    1

uint8_t rxbuffer[RXBUFFER];
uint8_t txbuffer[RXBUFFER];

float input_val, output_val;
float rx_value = 0.0;
__IO uint8_t rxcount = 0;

static void test_status_led_init(void);
uint8_t rxbuffer_analysis(uint8_t *rxbuffer);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0, 0);
    /* enable TMU clock */
    rcu_periph_clock_enable(RCU_TMU);
    /* configure the led GPIO */
    test_status_led_init();

    /* config the USART */
    gd_eval_com_init(EVAL_COM0);
    /* enable USART TBE interrupt */
    usart_interrupt_enable(USART0, USART_INT_RBNE);

    printf("\n\r TMU Caculation Test \n\r");
    printf("\n\r Please input any value between - 1 and 1: \n\r");
    /* wait the usart0 idle flag */
    while( !usart_flag_get(USART0, USART_FLAG_IDLE));
    usart_interrupt_disable(USART0, USART_INT_RBNE);
    
    if(DATA_ERRO == rxbuffer_analysis(rxbuffer)){
        printf("\n\r Input data is erro \n\r");
    }else{
        /* read the TMU calculation result */
        output_val = tmu_x_multiply_2pi_math(rx_value);
        /* check the TMU overflow flag */
        if(SET == tmu_flag_get(TMU_FLAG_OVRF)){
            gd_eval_led_on(LED1);
            gd_eval_led_on(LED2);
            printf("\n\r The TMU calculation is overflow:\n\r");
        }else{
            gd_eval_led_on(LED3);
            gd_eval_led_on(LED4);
            printf("\n\r The TMU calculation is:\n\r");
            printf("%11.9f",output_val);
        }
    }
    while(1);
}

/*!
    \brief      test status led initialize
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void test_status_led_init(void)
{
    /* initialize the leds */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* close all of leds */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      analysis data of rxbuffer
    \param[in]  rxbuffer: the buffer from usart0 recevie
    \param[out] none
    \retval     DATA_ERRO: the input data is erro
                DATA_OK: the input data is OK
*/
uint8_t rxbuffer_analysis(uint8_t *rxbuffer)
{
    uint8_t  i = 0;
    uint8_t  fraction_start = 0;
    uint32_t fraction_cnt = 1;
    uint8_t  fraction_flag = 0;
    int8_t   signal = 1;

    /* determine the sign of the input value */
    if(0x2D == rxbuffer[0]){
        signal = -1;
        /* The input value has fraction*/
        if(0x2E == rxbuffer[2]){
            fraction_flag = 1;
            fraction_start = 3;
        }
    }
    else if(0x2E == rxbuffer[1]){
        fraction_flag = 1;
        fraction_start = 2;
    }
    /* calculate the value of fraction */
    if(SET == fraction_flag){
        for(i=fraction_start;i<rxcount;i++){
            /*  */
            rx_value = rx_value*10+(rxbuffer[i]-0x30);
            fraction_cnt = fraction_cnt*10;
        }
        /* get the fraction of input value */
        rx_value = rx_value/fraction_cnt;
    }
    /* whether the input value is out of range */
    if(((0x31 <= rxbuffer[0])||(0x31 <= rxbuffer[1]))&&(rx_value > 0)){
        return DATA_ERRO;
    }else{
        /* get the sum of the decimal and fraction */
        if(SET == signal){
            rx_value = (float)(rx_value+(rxbuffer[0]-0x30));
        }
        else{
            rx_value = (float)(rx_value+(rxbuffer[1]-0x30));
        }
    }
    rx_value = signal*rx_value;
    return DATA_OK;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM0,USART_FLAG_TBE));
    return ch;
}
