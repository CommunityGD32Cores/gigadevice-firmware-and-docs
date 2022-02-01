/*!
    \file    main.c
    \brief   RTC calendar 
    
    \version 2017-12-26, V1.0.0, firmware for GD32E10x
    \version 2020-09-30, V1.1.0, firmware for GD32E10x
    \version 2020-12-31, V1.2.0, firmware for GD32E10x
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

#include "gd32e10x.h"
#include "gd32e10x_eval.h"
#include "gd32e10x_lcd_eval.h"
#include "rtc.h"
#include "systick.h"
#include <stdio.h>

#define BACKUP_DATA   (0xABCD)

void led_config(void);
void rtc_process(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* COM1 config */
    gd_eval_com_init(EVAL_COM1);

    /* LEDs config */
    led_config();

    /* configure the user key */
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);

    /* NVIC config */
    nvic_configuration();

    /* systick config */
    systick_config();

    /* initialize the LCD of GD EVAL board */
    gd_eval_lcd_init();

    /* clear the LCD screen */
    lcd_clear(WHITE);

    /* draw a rectangle */
    lcd_rectangle_draw(10, 10, 230, 310, BLUE);

    /* different RTC processes depending on the backup data to display calendar on LCD */
    rtc_process();

    /* clear reset flags */
    rcu_all_reset_flag_clear();

    while (1){
    }
}

/*!
    \brief      configure leds
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    gd_eval_led_init(LED5);
}

/*!
    \brief      different processes depending on the backup data to display calendar on LCD
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_process(void)
{
    printf("\r\n This is a RTC demo......" );

    if(bkp_data_read(BKP_DATA_0) != BACKUP_DATA){
        /* backup data register value is not correct or not yet programmed
        (when the first time the program is executed) */
        printf("\r\n RTC has not been configured yet....");

        /* RTC configuration */
        rtc_configuration();

        printf("\r\n start the RTC configuration....");

        /* adjust time by values entred by the user on the hyperterminal */
        time_adjust();

        bkp_data_write(BKP_DATA_0, BACKUP_DATA);
    }else{
        /* check if the power on/down reset flag is set */
        if(rcu_flag_get(RCU_FLAG_PORRST) != RESET){
            printf("\r\n\n Power On/Down Reset occurred....");
        }else if(rcu_flag_get(RCU_FLAG_SWRST) != RESET){
            /* check if the pin reset flag is set */
            printf("\r\n\n External Reset occurred....");
        }

        /* allow access to BKP domain */
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();
    
        printf("\r\n No need to configure RTC....");
        /* wait for RTC registers synchronization */
        rtc_register_sync_wait();
        rtc_lwoff_wait();
        /* enable the RTC second and alarm interrupt*/
        rtc_interrupt_enable(RTC_INT_SECOND);
        rtc_interrupt_enable(RTC_INT_ALARM);
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
