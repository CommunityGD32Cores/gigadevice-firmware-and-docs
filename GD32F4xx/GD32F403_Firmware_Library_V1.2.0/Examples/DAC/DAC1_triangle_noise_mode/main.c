/*!
    \file  main.c
    \brief DAC1 in triangle noise mode demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"

void rcu_config(void);
void gpio_config(void);
void dac_config(void);
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
    timer6_config();
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
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
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
    /* configure the DAC1 */
    dac_trigger_source_config(DAC1, DAC_TRIGGER_T6_TRGO);
    dac_trigger_enable(DAC1);
    dac_wave_mode_config(DAC1, DAC_WAVE_MODE_TRIANGLE);
    dac_triangle_noise_config(DAC1, DAC_TRIANGLE_AMPLITUDE_2047);
    dac_output_buffer_disable(DAC1);
    
    /* enable DAC1 and set data */
    dac_enable(DAC1);
    dac_data_set(DAC1, DAC_ALIGN_12B_R, 0x7F0);
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
