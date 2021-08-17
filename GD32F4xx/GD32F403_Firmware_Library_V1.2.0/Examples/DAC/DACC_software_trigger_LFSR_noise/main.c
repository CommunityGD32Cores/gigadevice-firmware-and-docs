/*!
    \file  main.c
    \brief DAC concurrent mode software trigger LFSR noise wave demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403.h"

void rcu_config(void);
void gpio_config(void);
void dac_config(void);

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
    while (1){
        dac_concurrent_software_trigger_enable();
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
    /* configure the DAC0 */
    dac_trigger_source_config(DAC0, DAC_TRIGGER_SOFTWARE);
    dac_trigger_enable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_MODE_LFSR);
    dac_lfsr_noise_config(DAC0, DAC_LFSR_BITS10_0);
    dac_output_buffer_disable(DAC0);
    
    /* configure the DAC1 */
    dac_trigger_source_config(DAC1, DAC_TRIGGER_SOFTWARE);
    dac_trigger_enable(DAC1);
    dac_wave_mode_config(DAC1, DAC_WAVE_MODE_LFSR);
    dac_lfsr_noise_config(DAC1, DAC_LFSR_BITS9_0);
    dac_output_buffer_disable(DAC1);
    
    /* enable DAC concurrent mode and set data */
    dac_concurrent_enable();
    dac_concurrent_data_set(DAC_ALIGN_12B_R, 0x600, 0x500);
}
