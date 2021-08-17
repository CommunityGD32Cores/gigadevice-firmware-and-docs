/*!
    \file    main.c
    \brief   DAC concurrent mode independent trigger with different LFSR demo

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

void rcu_config(void);
void gpio_config(void);
void dac_config(void);
void timer5_config(void);
void timer6_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    rcu_config();
    gpio_config();
    dac_config();
    timer5_config();
    timer6_config();

    while(1){
    }
}

/*!
    \brief      configure the RCU of peripherals
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable the clock of peripherals */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);
    rcu_periph_clock_enable(RCU_TIMER5);
    rcu_periph_clock_enable(RCU_TIMER6);
}

/*!
    \brief      configure the related GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* once enabled the DAC, the corresponding GPIO pin is connected to the DAC converter automatically */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
}

/*!
    \brief      configure the DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    dac_deinit();
    /* configure the DAC_OUT0 */
    dac_trigger_source_config(DAC_OUT_0, DAC_TRIGGER_T5_TRGO);
    dac_trigger_enable(DAC_OUT_0);
    dac_wave_mode_config(DAC_OUT_0, DAC_WAVE_MODE_LFSR);
    dac_lfsr_noise_config(DAC_OUT_0, DAC_LFSR_BITS9_0);

    /* configure the DAC_OUT1 */
    dac_trigger_source_config(DAC_OUT_1, DAC_TRIGGER_T6_TRGO);
    dac_trigger_enable(DAC_OUT_1);
    dac_wave_mode_config(DAC_OUT_1, DAC_WAVE_MODE_LFSR);
    dac_lfsr_noise_config(DAC_OUT_1, DAC_LFSR_BITS10_0);

    /* enable DAC concurrent mode and set data */
    dac_concurrent_enable();
    dac_concurrent_data_set(DAC_ALIGN_12B_R, 0x7F0, 0x7F0);
}

/*!
    \brief      configure the TIMER5
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer5_config(void)
{
    /* configure the TIMER5 */
    timer_prescaler_config(TIMER5, 0xF, TIMER_PSC_RELOAD_UPDATE);
    timer_autoreload_value_config(TIMER5, 0x1FF);
    timer_master_output_trigger_source_select(TIMER5, TIMER_TRI_OUT_SRC_UPDATE);

    timer_enable(TIMER5);
}

/*!
    \brief      configure the TIMER6
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer6_config(void)
{
    /* configure the TIMER6 */
    timer_prescaler_config(TIMER6, 0xF, TIMER_PSC_RELOAD_UPDATE);
    timer_autoreload_value_config(TIMER6, 0x1FF);
    timer_master_output_trigger_source_select(TIMER6, TIMER_TRI_OUT_SRC_UPDATE);

    timer_enable(TIMER6);
}
