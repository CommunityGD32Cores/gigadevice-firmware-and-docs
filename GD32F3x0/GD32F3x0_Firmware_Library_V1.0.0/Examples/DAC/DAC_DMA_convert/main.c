/*!
    \file  main.c
    \brief DAC DMA convert demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"

#define DAC_R8DH_ADDRESS 0x40007410
const uint8_t convertarr[10] = {0x00, 0x33, 0x66, 0x99, 0xCC, 0xFF, 0xCC, 0x99, 0x66, 0x33};

void rcu_config(void);
void gpio_config(void);
void dma_config(void);
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
    dma_config();
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
    rcu_periph_clock_enable(RCU_DMA);
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
    \brief      configure the DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_struct;
    /* clear all the interrupt flags */
    dma_flag_clear(DMA, DMA_CH2, DMA_FLAG_GIF);

    /* configure the DMA0 channel 2 */
    dma_struct.periph_addr = DAC_R8DH_ADDRESS;
    dma_struct.memory_addr = (uint32_t)convertarr;
    dma_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_struct.number = 10;
    dma_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_struct.priority = DMA_PRIORITY_ULTRA_HIGH;

    dma_circulation_enable(DMA_CH2);
    
    dma_init(DMA_CH2, dma_struct);
    
    dma_channel_enable(DMA_CH2);
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
    dac_wave_mode_config(DAC_WAVE_DISABLE);
    dac_output_buffer_disable();

    /* enable DAC and DMA for DAC */
    dac_enable();
    dac_dma_enable();
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
    timer_autoreload_value_config(TIMER5, 0xFF);
    timer_master_output_trigger_source_select(TIMER5, TIMER_TRI_OUT_SRC_UPDATE);
    
    timer_enable(TIMER5);
}
