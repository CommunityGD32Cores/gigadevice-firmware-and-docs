/*!
    \file    main.c
    \brief   Auto baudrate detection
    
    \version 2021-03-25, V1.0.0, firmware for GD32W51x
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

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{ 
    gd_eval_led_init(LED1);
    
    /* USART configuration */
    gd_eval_com_init(EVAL_COM0);
    
    printf("\n\rUSART auto baudrate detection example\n\r");
    /* configure the auto baudrate method */
    usart_autobaud_detection_mode_config(EVAL_COM0, USART_ABDM_FTOF);
    /* enable autobaudrate feature */
    usart_autobaud_detection_enable(EVAL_COM0);
    
    /* wait until receive enable acknowledge flag is set */
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_REA));
    
    /* wait until transmit enable acknowledge flag is set */
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TEA)); 
    
    /* loop until the end of auto baudrate phase */
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_ABD));
    
    /* if auto baudbate error occurred */
    if(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_ABDE)){
        /* wait until RBNE flag is set */
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE));
        
        /* wait until TBE flag is set */
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
        
        /* USART auto baudrate detection finished successfully */
        gd_eval_led_on(LED1);
        
        /* check the transfer complete flag */
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC));

        usart_transmit_config(EVAL_COM0, USART_TRANSMIT_DISABLE);
        usart_receive_config(EVAL_COM0, USART_RECEIVE_DISABLE);
    }
    
    /* USART disable */
    usart_disable(EVAL_COM0);
    while(1){
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}