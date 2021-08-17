/*!
    \file  main.c
    \brief TRNG get random

    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include <stdio.h>

char usart_data_get(void);
uint8_t trng_random_range_get(uint8_t min, uint8_t max);
ErrStatus trng_ready_check(void);  
ErrStatus trng_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t min,max;
    uint8_t retry = 0;
    
    /* USART configuration */
    gd_eval_com_init(EVAL_COM0);
    printf("\r\n /==============Gigadevice TRNG test=============/ \r\n");
    
    /* configure TRNG module */
    while((ERROR == trng_configuration()) && retry < 3){
        printf("TRNG init fail \r\n");
        printf("TRNG init retry \r\n");
        retry++;
    }
    
    if(3 == retry){
        /* if TRNG initialize fail */
        printf("TRNG init error \r\n");
        while(1);
    }
    /* TRNG initialize successf1ul */
    printf("TRNG init ok \r\n");

    while (1){   
        /* get the random number range */
        printf("Please input min num (hex format): \r\n");
        min = usart_data_get();
        printf("Please input max num (hex format): \r\n");
        max = usart_data_get();
        
        /* input value is invalid */
        if(min > max){
            printf("Please input correct num \r\n");
            continue;
        }
        
        printf("Input min num is %d \r\n", min);
        printf("Input max num is %d \r\n", max);
        printf("Generate random num1 is %d \r\n", trng_random_range_get(min, max));
        printf("Generate random num2 is %d \r\n", trng_random_range_get(min, max));
    }    
}

/*!
    \brief      check whether the TRNG module is ready
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus trng_ready_check(void)
{
    uint32_t timeout = 0;
    FlagStatus trng_flag = RESET;
    ErrStatus reval = SUCCESS;
    
    /* check wherther the random data is valid */
    do{
        timeout++;
        trng_flag = trng_flag_get(TRNG_FLAG_DRDY);
    }while((RESET == trng_flag) &&(0xFFFF > timeout));
    
    if(RESET == trng_flag)
    {   
        /* ready check timeout */
        printf("Error: TRNG can't ready \r\n");
        trng_flag = trng_flag_get(TRNG_FLAG_CECS);
        printf("Clock error current status: %d \r\n", trng_flag);
        trng_flag = trng_flag_get(TRNG_FLAG_SECS);
        printf("Seed error current status: %d \r\n", trng_flag);  
        reval = ERROR;
    }
    
    /* return check status */
    return reval;
}

/*!
    \brief      configure TRNG module
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus trng_configuration(void)
{
    ErrStatus reval = SUCCESS;
    
    /* TRNG module clock enable */
    rcu_periph_clock_enable(RCU_TRNG);
    
    /* TRNG registers reset */
    trng_deinit();
    trng_enable();
    /* check TRNG work status */
    reval = trng_ready_check();
    
    return reval;
}


/*!
    \brief      wait till a character is received by the USART
    \param[in]  none
    \param[out] none
    \retval     keyboard pressed character
*/
char usart_data_get(void)
{
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE)){
    }
    
    return (char)(usart_data_receive(EVAL_COM0)- 0x30); 
}

/*!
    \brief      get random data in range
    \param[in]  none
    \param[out] none
    \retval     random data
*/
uint8_t trng_random_range_get(uint8_t min, uint8_t max)
{     
    if(SUCCESS == trng_ready_check()){
        return (trng_get_true_random_data()%(max-min+1) + min);
    }else{
        return 0;
    } 
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC));
    
    return ch;
}
