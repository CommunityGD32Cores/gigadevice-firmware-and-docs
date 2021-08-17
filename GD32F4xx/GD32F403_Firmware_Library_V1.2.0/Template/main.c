/*!
    \file  main.c
    \brief led spark with systick, USART print and key example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f403_eval.h"

/*!
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if(timingdelaylocal){

        if(timingdelaylocal < 500U){
            gd_eval_led_on(LED1);
        }else{
            gd_eval_led_off(LED1);
        }

        timingdelaylocal--;
    }else{
        timingdelaylocal = 1000U;
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* configure systick */
    systick_config();
    /* initilize the LEDs, USART and key */
    gd_eval_led_init(LED1); 
    gd_eval_led_init(LED2); 
    gd_eval_led_init(LED3);
    gd_eval_com_init(EVAL_COM1);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));

    while (1){
        if(RESET == gd_eval_key_state_get(KEY_WAKEUP)){
            gd_eval_led_on(LED2);
            delay_1ms(500);
            gd_eval_led_off(LED2);
            gd_eval_led_toggle(LED3);
        }
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
