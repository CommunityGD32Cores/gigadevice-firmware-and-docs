/*!
    \file  main.c
    \brief DAC in LFSR noise mode demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "systick.h"

void rcu_config(void);
void gpio_config(void);
void dac_config(void);
void timer5_config(void);

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
    while (1){
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
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
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
    /* configure the DAC */
    dac_trigger_source_config(DAC_TRIGGER_T5_TRGO);
    dac_trigger_enable();
    dac_wave_mode_config(DAC_WAVE_MODE_LFSR);
    dac_lfsr_noise_config(DAC_LFSR_BITS10_0);
    dac_output_buffer_disable();
    
    /* enable DAC and set data */
    dac_enable();
    dac_data_set(DAC_ALIGN_12B_R, 0x7F0);
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
    timer_prescaler_config(TIMER5, 0x10, TIMER_PSC_RELOAD_UPDATE);
    timer_autoreload_value_config(TIMER5, 0x1FF);
    timer_master_output_trigger_source_select(TIMER5, TIMER_TRI_OUT_SRC_UPDATE);
    
    timer_enable(TIMER5);
}
