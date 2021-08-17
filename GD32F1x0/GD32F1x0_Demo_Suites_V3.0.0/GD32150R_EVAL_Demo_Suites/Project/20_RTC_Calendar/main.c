/*!
    \file  main.c
    \brief RTC calendar

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
#include "gd32f1x0_libopt.h"
#include "gd32f150r_eval.h"
#include "systick.h"
#include <stdio.h>

#include "lcd_driver.h"
#include "gui.h"
#include "tft_test.h"

#define RTC_CLOCK_SOURCE_IRC40K 

__IO uint32_t prescaler_a = 0, prescaler_s = 0;
rtc_parameter_struct rtc_initpara;

void led_flash(int times);
void rtc_pre_config(void);
void led_key_config(void);
void rcu_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    rcu_config();
    led_key_config();

    /* flash LEDs for test */
    led_flash(1);

    /* RTC configuration */
    rtc_pre_config();


    lcd_init();
    lcd_clear(BLUE);
    gui_draw_font_gbk24(10, 30, YELLOW, BLUE, "  Today is ");
    gui_draw_font_gbk16(2, 10, WHITE,BLUE, " GigaDevice Semiconductor Inc.");
    gui_draw_font_gbk16(2, 30, WHITE,BLUE, "  -- GD32F1x0 Series MCU --   ");    
    gui_draw_font_gbk16(2, 50, WHITE,BLUE, "     GD32F150R_EAVL  ");
    gui_draw_font_gbk16(2, 70, WHITE,BLUE, " RTC Test :");

    while( 1 )
    {
        /* get the current date & time, in BCD mode */
        rtc_current_time_get(&rtc_initpara);

        gui_draw_font_gbk24(10, 132, YELLOW, BLUE, "  Today is ");

        /* year */
        gui_draw_font_num32(20 - 15, 160, YELLOW, BLUE, 2 );
        gui_draw_font_num32(44 - 15, 160, YELLOW, BLUE, 0 );
        gui_draw_font_num32(68 - 15, 160, YELLOW, BLUE, rtc_initpara.rtc_year >> 4);
        gui_draw_font_num32(92 - 15, 160, YELLOW, BLUE, (rtc_initpara.rtc_year & 0x0F));  
        /* month */
        gui_draw_font_num32(120, 160, YELLOW, BLUE, rtc_initpara.rtc_month >> 4);
        gui_draw_font_num32(144, 160, YELLOW, BLUE, (rtc_initpara.rtc_month & 0x0F));
        /* date */
        gui_draw_font_num32(172, 160, YELLOW, BLUE, rtc_initpara.rtc_date >> 4);
        gui_draw_font_num32(196, 160, YELLOW, BLUE, (rtc_initpara.rtc_date & 0x0F));

        gui_draw_font_gbk24(10, 208, YELLOW, BLUE, "  Now Time is ");

        if(0 == rtc_initpara.rtc_am_pm){
            gui_draw_font_gbk24(20, 244, YELLOW, BLUE, "  AM ");
        }else{
            gui_draw_font_gbk24(20, 244, YELLOW, BLUE, "  PM ");
        }

        /* hour */
        gui_draw_font_num32(60, 236, YELLOW, BLUE, rtc_initpara.rtc_hour >> 4);
        gui_draw_font_num32(84, 236, YELLOW, BLUE, (rtc_initpara.rtc_hour & 0x0F));
        gui_draw_font_gbk24(112, 244, YELLOW, BLUE, ":");
        /* minute */
        gui_draw_font_num32(116, 236, YELLOW, BLUE, rtc_initpara.rtc_minute >> 4);
        gui_draw_font_num32(140, 236, YELLOW, BLUE, (rtc_initpara.rtc_minute & 0x0F));
        gui_draw_font_gbk24(164, 244, YELLOW, BLUE, ":");
        /* second */
        gui_draw_font_num32(168, 236, YELLOW, BLUE, rtc_initpara.rtc_second >> 4);
        gui_draw_font_num32(192, 236, YELLOW, BLUE, (rtc_initpara.rtc_second & 0x0F));
        
        if(0 == gd_eval_key_state_get(KEY_WAKEUP)){
            delay_1ms(50);
      
            if(0 == gd_eval_key_state_get(KEY_WAKEUP)){ 
                delay_1ms(50);
    
                if(0 == gd_eval_key_state_get(KEY_WAKEUP)){ 
                    rtc_initpara.rtc_year        = 0x16 ;
                    rtc_initpara.rtc_month       = RTC_MAY ;
                    rtc_initpara.rtc_date        = 0x13 ;
                    rtc_initpara.rtc_day_of_week = RTC_FRIDAY ;
                    rtc_initpara.rtc_hour        = 0x12;
                    rtc_initpara.rtc_minute      = 0x00;
                    rtc_initpara.rtc_second      = 0x00;
                    rtc_initpara.rtc_display_format = RTC_24HOUR;
  
                    rtc_init(&rtc_initpara);                
                }
            }
        } 
    }
}

/*!
    \brief      LEDs and KEYs configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_key_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* test key init */
    gd_eval_key_init(KEY_WAKEUP,KEY_MODE_GPIO);
}

/*!
    \brief      RCU configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);
}

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
    /* enable access to RTC registers in backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    #if defined (RTC_CLOCK_SOURCE_IRC40K)    
          /* enable the IRC40K oscillator */
          rcu_osci_on(RCU_IRC40K);
          /* wait till IRC40K is ready */
          rcu_osci_stab_wait(RCU_IRC40K);
          /* select the RTC clock source */
          rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
  
          prescaler_s = 0x18F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
           /* enable the IRC40K oscillator */
          rcu_osci_on(RCU_LXTAL);
          /* wait till IRC40K is ready */
          rcu_osci_stab_wait(RCU_LXTAL);
          /* select the RTC clock source */
          rcu_rtc_clock_config(RCU_LXTAL);
          
          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC40K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      test status led
    \param[in]  times: specifies the delay time length, in milliseconds
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    
    for(i = 0; i < times; i++){
        /* insert 200 ms delay */
        delay_1ms(200);
    
        /* turn on LEDs */
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
