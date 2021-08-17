/*!
    \file    main.c
    \brief   RTC clock calibration
    
    \version 2020-12-31, V1.0.0, firmware for GD32C10x
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

#include "gd32c10x.h"
#include "gd32c10x_eval.h"
#include <stdio.h>
#include "main.h"

/* timer capture times and values */
__IO uint32_t timer_capture_num = 0;
__IO uint32_t timer_capture1 = 0;
__IO uint32_t timer_capture2 = 0;

/* configure the nested vectored interrupt controller */
void nvic_configuration(void);
/* configure the RTC */
void rtc_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint32_t timer_period  = 0, irc_freq = 0;
    uint32_t temp_capture1 = 0, temp_capture2 = 0;

    timer_capture_num = 0;

    /* LED config */
    gd_eval_led_init(LED2);

    /* COM1 config */
    gd_eval_com_init(EVAL_COM1);

    /* configure the user key */
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);

    /* NVIC config */
    nvic_configuration();

    /* RTC configuration */
    rtc_configuration();

    /* connect IRC40K clock to the TIMER4_CH3 input for calibration */
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_TIMER4CH3_IREMAP, ENABLE);

    /* capture 2 times */
    while(2 != timer_capture_num){
    }

    temp_capture1 = timer_capture1;
    temp_capture2 = timer_capture2;
    timer_period = temp_capture2 - temp_capture1 + 1;

    /* compute the actual frequency of the IRC40K (TIMER4_CLK = 2 * CK_APB1) */
    if(0 != timer_period){
        irc_freq = ((rcu_clock_freq_get(CK_APB1) * 2) / timer_period);
    }

    printf("timer_capture1 = %d, timer_capture2 = %d, irc_freq = %d\n", temp_capture1, temp_capture2, irc_freq);

    /* adjust the RTC prescaler value to calibrate the clock */
    rtc_lwoff_wait();
    rtc_prescaler_set(irc_freq - 1);
    rtc_lwoff_wait();

    while(1){
    }
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(RTC_IRQn,1,0);
    nvic_irq_enable(TIMER4_IRQn,1,1);
}

/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
    
    /* select RCU_IRC40K as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);

    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* enable the RTC second interrupt*/
    rtc_interrupt_enable(RTC_INT_SECOND);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    rtc_prescaler_set(40000);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* disable tamper detection function to output the RTC second signal on tamper pin */
    bkp_tamper_detection_disable();

    /* select and enable the RTC second signal output on tamper pin for measuring */
    bkp_rtc_output_select(RTC_OUTPUT_SECOND_PULSE);
    bkp_rtc_signal_output_enable();
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* TIMER4 configuration: input capture mode -------------------
    the RTC signal is connected to TIMER4 CH3
    the rising edge is used as active edge
    the TIMER4 CH3CV is used to compute the frequency value
    ------------------------------------------------------------ */
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER4);

    timer_deinit(TIMER4);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER4 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 0xFFFF;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER4, &timer_initpara);

    /* TIMER4  configuration */
    /* initialize TIMER channel input parameter struct */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    /* TIMER4 CH3 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER4, TIMER_CH_3, &timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER4);
    /* clear channel 3 interrupt bit */
    timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_CH3);
    /* channel 3 interrupt enable */
    timer_interrupt_enable(TIMER4, TIMER_INT_CH3);

    /* TIMER4 counter enable */
    timer_enable(TIMER4);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
