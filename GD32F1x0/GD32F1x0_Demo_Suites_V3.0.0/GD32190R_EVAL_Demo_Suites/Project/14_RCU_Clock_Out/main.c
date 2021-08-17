/*!
    \file  main.c
    \brief GPIO Key Polling demo

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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
#include "systick.h"
#include "gd32f190r_eval.h"


#define BOARD_LED_NUMBER    4

typedef enum
{
    CKOUT_IRC14M = 0,
    CKOUT_IRC40K,
    CKOUT_LXTAL,
    CKOUT_SYS,
    CKOUT_IRC8M,
    CKOUT_HXTAL,
    CKOUT_PLL
}clock_output_enum;

void all_led_init(void);
void all_led_off(void);
void clock_output_select(uint8_t seq);
void clock_output_config(void);
void led_indicate(uint8_t seq);

extern uint8_t g_button_press_flag;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t func_seq = 0;
    
    /* initialize the USART */
    gd_eval_com_init(EVAL_COM1);
    
    printf("\r\n /=========== Gigadevice Clock output Demo ===========/ \r\n");
    printf("press wakeup key to select clock output source \r\n");
    
    /* initialize the LEDs */
    all_led_init();
    /* initialize the clock output */
    clock_output_config();
    /* initialize the wakeup key */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    /* configure the systick */
    systick_config();
   
    while (1){
        if(1 == g_button_press_flag){
            /* if the button is pressed */
            g_button_press_flag = 0;
            /* control the led */
            all_led_off();
            led_indicate(func_seq);
            /* select the clock output mode */
            clock_output_select(func_seq);
            
            func_seq++;
            func_seq %= 7;
        }
    }
}

/*!
    \brief      led show
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_indicate(uint8_t seq)
{
    uint8_t add;
    
    add = seq;
    add /= BOARD_LED_NUMBER;
    
    if(add == 0){
        gd_eval_led_on((led_typedef_enum)seq);
    }else if(add == 1){
        seq %= BOARD_LED_NUMBER;
        gd_eval_led_on(LED1);
        gd_eval_led_on((led_typedef_enum)(seq + add));
    }else{
        printf("\r\n LED seq value wrong \r\n");
    }
}

/*!
    \brief      all LEDs turn off
    \param[in]  none
    \param[out] none
    \retval     none
*/
void all_led_off(void)
{
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      configure clock output function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void clock_output_config(void)
{
    /* peripheral clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_PMU);
    /* backup domain write enable */
    pmu_backup_write_enable();
    rcu_lxtal_drive_capability_config(RCU_LXTAL_HIGHDRI);
    rcu_osci_on(RCU_LXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_LXTAL)){    
        printf("RCU_LXTAL rcu_osci_stab_wait timeout! \r\n");
        while(1);
    }
    
    rcu_osci_on(RCU_IRC28M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC28M)){
        printf("RCU_IRC28M rcu_osci_stab_wait timeout! \r\n");
        while(1);
    }
    
    rcu_osci_on(RCU_IRC40K);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC40K)){    
        printf("RCU_IRC40K rcu_osci_stab_wait timeout! \r\n");
        while(1);
    }
    
    /* configure clock output pin */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8);
}

/*!
    \brief      select type of clock output
    \param[in]  none
    \param[out] none
    \retval     none
*/
void clock_output_select(uint8_t seq)
{
    switch(seq){
        case CKOUT_IRC14M:
            rcu_ckout0_config(RCU_CKOUT0SRC_IRC28M, RCU_CKOUT0_DIV1);
            printf("CK_OUT0: IRC28M, DIV:1 \r\n");
            break;
        case CKOUT_IRC40K:
            rcu_ckout0_config(RCU_CKOUT0SRC_IRC40K, RCU_CKOUT0_DIV1);
            printf("CK_OUT0: IRC40K, DIV:1 \r\n");
            break;
        case CKOUT_LXTAL:
            rcu_ckout0_config(RCU_CKOUT0SRC_LXTAL, RCU_CKOUT0_DIV1);
            printf("CK_OUT0: LXTAL, DIV:1 \r\n");
            break;
        case CKOUT_SYS:
            rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS, RCU_CKOUT0_DIV4);
            printf("CK_OUT0: CKSYS, DIV:4 \r\n");
            break;   
        case CKOUT_IRC8M:
            rcu_ckout0_config(RCU_CKOUT0SRC_IRC8M, RCU_CKOUT0_DIV1);
            printf("CK_OUT0: IRC8M, DIV:1 \r\n");
            break;
        case CKOUT_HXTAL:
            rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);
            printf("CK_OUT0: HXTAL, DIV:1 \r\n");
            break;
        case CKOUT_PLL:
            rcu_ckout0_config(RCU_CKOUT0SRC_CKPLL_DIV1, RCU_CKOUT0_DIV4);
            printf("CK_OUT0: CKPLL, DIV:4 \r\n");
            break;
        default:
            printf("clock output select parameter error! \r\n");
            while(1);
    }
}

/*!
    \brief      initialize the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void all_led_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TC));
    
    return ch;
}
