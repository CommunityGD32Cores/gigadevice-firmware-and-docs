/*!
    \file  main.c
    \brief DAC0 in EXTI trigger mode demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"

void rcu_config(void);
void gpio_config(void);
void dac_config(void);
void exti_line9_config(void);

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
    exti_line9_config();
    
    while (1){
    }
}

/*!
    \brief      configure the EXTI interrupt line 9. PE9 is used for an example.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exti_line9_config(void)
{
    /* enable the key clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin as input */
    gpio_init(GPIOE, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* enable and set key EXTI interrupt priority */
    nvic_irq_enable(EXTI5_9_IRQn, 1U, 0U);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_9);

    /* configure key EXTI line */
    exti_init(EXTI_9, EXTI_EVENT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_9);
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
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
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
    dac_trigger_source_config(DAC0, DAC_TRIGGER_EXTI_9);
    dac_trigger_enable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_MODE_LFSR);
    dac_lfsr_noise_config(DAC0, DAC_LFSR_BITS11_0);
    dac_output_buffer_disable(DAC0);

    /* enable DAC0 and set data */
    dac_enable(DAC0);
    dac_data_set(DAC0, DAC_ALIGN_12B_R, 0);
}

