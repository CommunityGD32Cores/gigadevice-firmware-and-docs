/*!
    \file    main.c
    \brief   SHRTIMER0 input capture

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include <stdio.h>
#include "main.h"
#include "gd32e507z_eval.h"
#include "systick.h"

extern uint32_t percap0;

void gpio_config(void);
void shrtimer_config(void);
int fputc(int ch, FILE *f);

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* SHRTIMER_EXEV5(PB5) */
    gpio_afio_port_config(AFIO_PB5_SHRTIMER_CFG, ENABLE);
    gpio_init(GPIOB,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_5);
}

/*!
    \brief      configure the SHRTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void shrtimer_config(void)
{
/* -----------------------------------------------------------------------
    fSHRTIMER_CK: 180MHz, from CK_SYS
    fSHRTIMER_PSCCK: 180MHz
    external signal cycle range: (1/180us)~(65535/180us)
  ----------------------------------------------------------------------- */
    shrtimer_baseinit_parameter_struct baseinit_para;
    shrtimer_exeventcfg_parameter_struct exevcfg_para;
    shrtimer_timercfg_parameter_struct timercfg_para;

    /* clock config */
    rcu_periph_clock_enable(RCU_SHRTIMER);
    rcu_shrtimer_clock_config(RCU_SHRTIMERSRC_CKSYS);

    /* periodic DLL calibration */
    shrtimer_dll_calibration_start(SHRTIMER0, SHRTIMER_CALIBRATION_16384_PERIOD);
    while(RESET == shrtimer_common_flag_get(SHRTIMER0, SHRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    shrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period = 0xFFFF;
    baseinit_para.prescaler = SHRTIMER_PRESCALER_DIV1;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode = SHRTIMER_COUNTER_MODE_CONTINOUS;
    shrtimer_timers_base_init(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &baseinit_para);

    /* configure the general behavior of a Slave_TIMER0 which work in waveform mode */
    shrtimer_timercfg_struct_para_init(&timercfg_para);
    timercfg_para.balanced_mode = SHRTIMER_STXBALANCEDMODE_DISABLED;
    timercfg_para.cnt_reset = SHRTIMER_STXCNT_RESET_EEV_5;
    timercfg_para.deadtime_enable = SHRTIMER_STXDEADTIME_DISABLED;
    timercfg_para.delayed_idle = SHRTIMER_STXDELAYED_IDLE_DISABLED;
    timercfg_para.fault_enable = SHRTIMER_STXFAULTENABLE_NONE;
    timercfg_para.fault_protect = SHRTIMER_STXFAULT_PROTECT_READWRITE;
    timercfg_para.reset_update = SHRTIMER_STXUPDATEONRESET_DISABLED;
    timercfg_para.update_source = SHRTIMER_STXUPDATETRIGGER_NONE;
    shrtimer_slavetimer_waveform_config(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &timercfg_para);

    /* configures the an external event */
    shrtimer_exeventcfg_struct_para_init(&exevcfg_para);
    exevcfg_para.digital_filter = 0x1;
    exevcfg_para.edge = SHRTIMER_EXEV_EDGE_FALLING;
    exevcfg_para.polarity = SHRTIMER_EXEV_EDGE_LEVEL;
    exevcfg_para.source = SHRTIMER_EXEV_SRC0;
    shrtimer_exevent_config(SHRTIMER0, SHRTIMER_EXEVENT_5, &exevcfg_para);

    /* configure the capture source in Slave_TIMER */
    shrtimer_slavetimer_capture_config(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, SHRTIMER_CAPTURE_0, SHRTIMER_CAPTURETRIGGER_EXEV_5);
    /* clear the Master_TIMER and Slave_TIMER interrupt flag */
    shrtimer_timers_interrupt_flag_clear(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, SHRTIMER_ST_INT_FLAG_CAP0);
    /* enable the Master_TIMER and Slave_TIMER interrupt */
    shrtimer_timers_interrupt_enable(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, SHRTIMER_ST_INT_CAP0);

    /* enable NVIC request */
    nvic_irq_enable(SHRTIMER_IRQ1_IRQn, 0U, 0U);
    /* enable a counter */
    shrtimer_timers_counter_enable(SHRTIMER0, SHRTIMER_ST0_COUNTER);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM0);
    systick_config();
    gpio_config();
    shrtimer_config();

    while(1){
        delay_1ms(10);
        printf("the period of external signal is %d ns \n", percap0);
    }
}
