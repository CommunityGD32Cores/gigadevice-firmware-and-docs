/*!
    \file  main.c
    \brief RTC carlendar demo

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
#include "gd32f190r_eval.h"
#include "slcd_seg.h"
#include "systick.h"

__IO uint32_t asynchprediv = 0, synchprediv = 0;

rtc_parameter_struct   rtc_initpara;

void flash_led(int times);
void test_key_init(void);
void rtc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t i = 0;
    /* init the systick */
    systick_config();

    /* init test status led */
    gd_eval_led_init(LED1);

    /* flash led for test */
    flash_led(1);

    /* test key init */
    test_key_init();

    /* RTC config */
    rtc_config();

    /* init the SLCD */
    slcd_seg_configuration();

    /* infinite loop */
    while(1){
        rtc_current_time_get(&rtc_initpara);
        if(0 == gd_eval_key_state_get(KEY_TAMPER)){
            delay_1ms(10);

            if(0 == gd_eval_key_state_get(KEY_TAMPER)){
                delay_1ms(10);

                if(0 == gd_eval_key_state_get(KEY_TAMPER)){
                    i++;
                    i %= 3;
                }
            }
        }

        if(0 == i){
            slcd_seg_digit_display((rtc_initpara.rtc_hour >> 4),1);
            slcd_seg_digit_display((rtc_initpara.rtc_hour & 0x0F),2);

            slcd_seg_digit_display((rtc_initpara.rtc_minute >> 4),3);
            slcd_seg_digit_display((rtc_initpara.rtc_minute & 0x0F),4);

            slcd_seg_digit_display((rtc_initpara.rtc_second >> 4),5);
            slcd_seg_digit_display((rtc_initpara.rtc_second & 0x0F),6);
        }else if(1 == i){
            slcd_seg_number_display(2000 + (rtc_initpara.rtc_year >> 4)*10 + (rtc_initpara.rtc_year & 0x0F));
        }else if(2 == i){
            slcd_seg_digit_clear(4);
            slcd_seg_digit_display((rtc_initpara.rtc_month >> 4),2);
            slcd_seg_digit_display((rtc_initpara.rtc_month & 0x0F),3);
            slcd_seg_digit_display((rtc_initpara.rtc_date >> 4),5);
            slcd_seg_digit_display((rtc_initpara.rtc_date & 0x0F),6);
        }
        delay_1ms(300);
    }
}

/*!
    \brief      configure the rtc peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_config( void )
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* PMU backup domain write enable */
    pmu_backup_write_enable();
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait for IRC40K stabilization flags */
    rcu_osci_stab_wait(RCU_IRC40K);
    /* configure the RTC clock source selection */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
    /* enable RTC clock */
    rcu_periph_clock_enable(RCU_RTC);

    synchprediv = 0x18F;
    asynchprediv = 0x63;

    rtc_initpara.rtc_factor_asyn = asynchprediv;
    rtc_initpara.rtc_factor_syn = synchprediv;
    rtc_initpara.rtc_year = 0x16;
    rtc_initpara.rtc_day_of_week = RTC_FRIDAY;
    rtc_initpara.rtc_month = RTC_MAY;
    rtc_initpara.rtc_date = 0x13;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;
    rtc_initpara.rtc_hour = 0;
    rtc_initpara.rtc_minute = 0;
    rtc_initpara.rtc_second = 0;

    rtc_init(&rtc_initpara);

}

/*!
    \brief      led blink function
    \param[in]  times: the blink times of leds
    \param[out] none
    \retval     none
*/
void flash_led(int times)
{
    int i;

    for(i = 0;i < times;i++){
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on led */
        gd_eval_led_on(LED1);

        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off led */
        gd_eval_led_off(LED1);
    }
}

/*!
    \brief      test key init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_key_init( void )
{
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);
}
