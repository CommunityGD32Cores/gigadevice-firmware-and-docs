/*!
    \file    main.c
    \brief   USART transmit and receive interrupt
    
    \version 2023-12-31, V3.5.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE   (ARRAYNUM(transmitter_buffer) - 1)

uint8_t transmitter_buffer[] = "\n\rUSART interrupt test\n\r";
uint8_t receiver_buffer[32];
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 32;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 


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
     
    gd_eval_com_init(EVAL_COM0);
    
    /* enable USART TBE interrupt */  
    usart_interrupt_enable(EVAL_COM0, USART_INT_TBE);
    
    /* wait until USART send the transmitter_buffer */
    while(txcount < transfersize) {
	}
    
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC)) {
	}
    
    usart_interrupt_enable(EVAL_COM0, USART_INT_RBNE);
    
    /* wait until USART receive the receiver_buffer */
    while(rxcount < receivesize) {
	}
    if(rxcount == receivesize)
        printf("\n\rUSART receive successfully!\n\r");
      
    while (1) {
	}
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE)) {
	}
    return ch;
}
