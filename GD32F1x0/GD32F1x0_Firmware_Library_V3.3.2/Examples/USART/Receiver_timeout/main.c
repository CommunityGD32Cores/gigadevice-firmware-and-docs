/*!
    \file    main.c
    \brief   UASRT receiver timeout
    
    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h"
#include <stdio.h>
#include "gd32f1x0r_eval.h"

uint8_t rxbuffer[64];
uint8_t txbuffer[64];
extern __IO uint8_t txcount; 
extern __IO uint16_t rxcount; 
void nvic_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t i=0, j=0;
    nvic_config();
    
#ifdef GD32F130_150
    gd_eval_com_init(EVAL_COM0);
    printf("a usart receive timeout test example!");
    
    while (1){
        if(0 == rxcount){
            /* enable the USART receive interrupt */
            usart_interrupt_enable(EVAL_COM0, USART_INT_RBNE);
        }else{
            /* enable the USART receive timeout and configure the time of timeout */
            usart_receiver_timeout_enable(EVAL_COM0);
            usart_receiver_timeout_threshold_config(EVAL_COM0, 115200*3);

            while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RT));
            
            for(i=0; i<rxcount; i++){ 
                txbuffer[i]=rxbuffer[j++];
            } 
            /* disable the USART receive interrupt and enable the USART transmit interrupt */
            usart_interrupt_disable(EVAL_COM0, USART_INT_RBNE);
            usart_interrupt_enable(EVAL_COM0, USART_INT_TBE);
            while(txcount < rxcount);
            usart_flag_clear(EVAL_COM0, USART_FLAG_RT);
            txcount=0;
            rxcount=0; 
            i=0;
            j=0;
        }
    }
#elif GD32F170_190
    gd_eval_com_init(EVAL_COM1);
    printf("a usart receive timeout test example!");
    
    while (1){
        if(0 == rxcount){
            /* enable the USART receive interrupt */
            usart_interrupt_enable(EVAL_COM1, USART_INT_RBNE);
        }else{
            /* enable the USART receive timeout and configure the time of timeout */
            usart_receiver_timeout_enable(EVAL_COM1);
            usart_receiver_timeout_threshold_config(EVAL_COM1, 115200*3);

            while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_RT));
            
            for(i=0; i<rxcount; i++){ 
                txbuffer[i]=rxbuffer[j++];
            } 
            /* disable the USART receive interrupt and enable the USART transmit interrupt */
            usart_interrupt_disable(EVAL_COM1, USART_INT_RBNE);
            usart_interrupt_enable(EVAL_COM1, USART_INT_TBE);
            while(txcount < rxcount);
            usart_flag_clear(EVAL_COM1, USART_FLAG_RT);
            txcount=0;
            rxcount=0; 
            i=0;
            j=0;
        }
    }
#endif /*G32F170_190*/
}

#ifdef GD32F130_150
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
#elif defined GD32F170_190
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));
    return ch;
}
#endif /* GD32F130_150 */

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(USART0_IRQn, 0, 1);
}
