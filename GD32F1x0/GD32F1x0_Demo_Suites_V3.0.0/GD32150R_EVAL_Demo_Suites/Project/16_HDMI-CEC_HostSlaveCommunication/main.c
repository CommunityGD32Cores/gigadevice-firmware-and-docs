/*!
    \file  main.c
    \brief CEC intercommunication

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
#include <stdlib.h>
#include <stdio.h>
#include "gd32f150r_eval.h"
#include "lcd_driver.h"
#include "gui.h"
#include "systick.h"

extern uint8_t rcvdata[10];
extern __IO uint8_t rcvstatus;
extern uint8_t rcv_inc;
extern uint8_t bytenum;
__IO uint8_t framenum=0;

void cec_config(void);
void test_display_init(void);
void led_config(void);
void flash_led(uint8_t times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure the systick */
    systick_config();
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure LEDs */
    led_config();

    /* flash LEDs once */
    flash_led(1);    

    /* TFT_LCD initialize */
    lcd_init();
    lcd_clear(BLUE);

    /* TFT_LCD display "Start..." */
    test_display_init();

    /* configure the CEC peripheral */
    cec_config();

    /* configure the EXTI */
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);


    /* main loop */
    while(1){
        /* wait receive data */
        while(0 == rcvstatus);

        if(1 == rcvstatus){
            /* if tamper key press */
            if((0xA5 == rcvdata[1])&&(0x5A == rcvdata[2])){
                framenum++;
            }
            /* if user key press */
            if((0x5A == rcvdata[1])&&(0xA5 == rcvdata[2])){
                framenum--;
            }
            /* if framenum==0,then decrement 1,frame==FF */
            if(0xFF == framenum){
                framenum=9;
            }
            /* if framenum++ to 10 */
            if(10 == framenum){
                framenum=0;
            }
            gui_draw_font_num32(80,160,BLUE,GREEN,framenum);
        }else{
            /* a reception error occured */
            gui_draw_font_gbk16(16,120, BLUE,GREEN,(char *)("reception error!")); 
        }
        rcvstatus = 0;
    }
}

/*!
    \brief      configure the CEC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_config(void)
{
    /* enable clocks */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_CEC);

    /* configure CEC_LINE_GPIO as output open drain */
    gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_8);
    gpio_output_options_set(GPIOB,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_PIN_8);
    gpio_af_set(GPIOB,GPIO_AF_0,GPIO_PIN_8);

    /* configure priority group */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    /* enable the CEC global interrupt (with higher priority) */
    nvic_irq_enable(CEC_IRQn,0,0);

    /* configure CEC */
    cec_init(CEC_SFT_START_STAOM,CEC_SFT_1POINT5_PERIOD,CEC_OWN_ADDRESS2);
    cec_error_config(CEC_BROADCAST_ERROR_BIT_OFF,CEC_LONG_PERIOD_ERROR_BIT_OFF,CEC_RISING_PERIOD_ERROR_BIT_OFF,CEC_STOP_RISING_ERROR_BIT_OFF);
    cec_reception_tolerance_disable();

    /* activate CEC interrupts associated to the set of TX and RX flags */
    cec_interrupt_enable(CEC_INTEN_TENDIE | CEC_INTEN_TBRIE | CEC_INTEN_RENDIE | CEC_INTEN_BRIE);

    /* activate CEC interrupts associated to the set of TX and RX error */
    cec_interrupt_enable(CEC_INTEN_ROIE | CEC_INTEN_BREIE | CEC_INTEN_BPSEIE | CEC_INTEN_BPLEIE 
    | CEC_INTEN_RAEIE | CEC_INTEN_ARBFIE | CEC_INTEN_TUIE | CEC_INTEN_TERRIE | CEC_INTEN_TAERRIE );

    /* enable CEC */
    cec_enable();
}

/*!
    \brief      display welcome message
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_display_init(void)
{
    #define x_offset 2
    {
        gui_draw_font_gbk16( x_offset,10,WHITE,BLUE, (char *)" GigaDevice Semiconductor Inc.");
        gui_draw_font_gbk16( x_offset,30,WHITE,BLUE, (char *)"  -- GD32F1x0 Series MCU --   ");
        gui_draw_font_gbk16( x_offset,50,WHITE,BLUE, (char *)"     GD32F150R_EAVL_V1.3  ");
        gui_draw_font_gbk16( x_offset,70,WHITE,BLUE, (char *)" HDMI-CEC Test :");
        gui_draw_font_gbk16(16,90, WHITE,BLUE,(char *)(" reception counter ")); 
        gui_draw_font_num32(80,160,WHITE,BLUE,framenum);
    }
}

/*!
    \brief      configure led
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    /* initialize the LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* close all of LEDs */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
    
    /* setup systick timer for 1 msec interrupts */
    systick_config();
}

/*!
    \brief      test status leds
    \param[in]  times:leds blink times 
    \param[out] none
    \retval     none
*/
void flash_led(__IO uint8_t times)
{
    int i;

    for(i = 0; i < times; i++){
        /* insert 200 ms delay */
        delay_1ms(200);

        /* initialize the leds */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off LEDs */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}
