/*!
    \file  main.c
    \brief DAC output voltage
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include <stdio.h>

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
    while(1);
}

/*!
    \brief      configure RCU function
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
    \brief      configure GPIO function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* once enabled the DAC, the corresponding GPIO pin is connected to the DAC converter automatically */
    gpio_mode_set(GPIOA,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_4);
}

/*!
    \brief      configure DAC function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    dac_deinit();
    /* configure the DAC */
    dac_trigger_source_config(DAC_TRIGGER_SOFTWARE);
    dac_output_buffer_disable();
    /* set data left 12b alignment */
    dac_data_set(DAC_ALIGN_12B_L,0x7FF0);
    /* enable DAC */
    dac_enable();
    dac_software_trigger_enable();
}
