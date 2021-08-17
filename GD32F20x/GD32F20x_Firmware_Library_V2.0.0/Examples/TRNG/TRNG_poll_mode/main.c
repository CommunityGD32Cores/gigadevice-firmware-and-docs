/*!
    \file  main.c
    \brief the example of TRNG 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include "gd32f207i_eval.h"
#include <stdio.h>

FlagStatus g_tamperbutton_flag = RESET;
ErrStatus trng_configuration(void);
ErrStatus trng_ready_check(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t random_data = 0, random_lastdata = 0;
    uint8_t retry = 0;
    
    /* initialize the LEDs and turn on LED1 */
    gd_eval_led_init(LED1);
    gd_eval_led_on(LED1);
    gd_eval_com_init(EVAL_COM1);
    
    /* configure the tamper key */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    
    /* show example introduce */
    printf("============ Gigadevice TRNG poll mode demo ============ \r\n");
    
    /* configure TRNG module */
    while((ERROR == trng_configuration()) && retry < 3){
        printf("TRNG init fail \r\n");
        printf("TRNG init retry \r\n");
        retry++;
    }
    
    printf("TRNG init ok \r\n");
    /* get the first random data */
    random_lastdata = trng_get_true_random_data();
    
    while(1){
        /* get random data if TEMPER key is pressed */
        if(SET == g_tamperbutton_flag){
            g_tamperbutton_flag = RESET;
            /* check wherther the random data is valid and get it */
            if(SUCCESS == trng_ready_check()){
                random_data = trng_get_true_random_data();
                if(random_data != random_lastdata){
                    random_lastdata = random_data;
                    printf("Get random data: 0x%08x \r\n", random_data);
                }else{
                    /* the random data is invalid */
                    printf("Error: Get the random data is same \r\n");
                }
            }
        }
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
    
    /* check wherther the random data is valid */
    do{
        timeout++;
        trng_flag = trng_flag_get(TRNG_FLAG_DRDY);
    }while((RESET == trng_flag) &&(0xFFFF > timeout));
    
    if(RESET == trng_flag){   
        /* ready check timeout */
        printf("Error: TRNG can't ready \r\n");
        return ERROR;
    }
    
    trng_flag = trng_flag_get(TRNG_FLAG_CECS);
    if(SET == trng_flag){
        printf("Clock error status: %d \r\n", trng_flag);
        return ERROR;
    }
        
    trng_flag = trng_flag_get(TRNG_FLAG_SECS);
    if(SET == trng_flag){
        printf("Seed error status: %d \r\n", trng_flag);      
        return ERROR;     
    }
    
    /* return check status */
    return SUCCESS;
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

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TC));
    
    return ch;
}
