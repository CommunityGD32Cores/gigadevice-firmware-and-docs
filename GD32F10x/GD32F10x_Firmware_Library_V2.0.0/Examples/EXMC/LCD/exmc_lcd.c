/*!
    \file  exmc_lcd.c
    \brief EXMC LCD driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "gd32f10x.h"
#include "exmc_lcd.h"

/*!
    \brief      lcd peripheral initialize
    \param[in]  none 
    \param[out] none
    \retval     none
*/
void exmc_lcd_init(void)
{
    exmc_norsram_parameter_struct lcd_init_struct;
    exmc_norsram_timing_parameter_struct lcd_timing_init_struct;

    /* EXMC clock enable */
    rcu_periph_clock_enable(RCU_EXMC);
    rcu_periph_clock_enable(RCU_AF);
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure EXMC_D[0~15]*/
    /* PD14(EXMC_D0), PD15(EXMC_D1),PD0(EXMC_D2), PD1(EXMC_D3), PD8(EXMC_D13), PD9(EXMC_D14), PD10(EXMC_D15) */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1| GPIO_PIN_8 | GPIO_PIN_9 |
                                                         GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15);

    /* PE7(EXMC_D4), PE8(EXMC_D5), PE9(EXMC_D6), PE10(EXMC_D7), PE11(EXMC_D8), PE12(EXMC_D9), 
       PE13(EXMC_D10), PE14(EXMC_D11), PE15(EXMC_D12) */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | 
                                                         GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | 
                                                         GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* configure PE2(EXMC_A23) */ 
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    
    /* configure NOE and NWE */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);

    /* configure EXMC NE1 */
    gpio_init(GPIOD, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    lcd_timing_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;
    lcd_timing_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;
    lcd_timing_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_DISABLE;
    lcd_timing_init_struct.bus_latency = 0;
    lcd_timing_init_struct.asyn_data_setuptime = 5;
    lcd_timing_init_struct.asyn_address_holdtime = 0;
    lcd_timing_init_struct.asyn_address_setuptime = 2;

    lcd_init_struct.norsram_region = EXMC_BANK0_NORSRAM_REGION0;
    lcd_init_struct.write_mode = EXMC_ASYN_WRITE;
    lcd_init_struct.extended_mode = DISABLE;
    lcd_init_struct.asyn_wait = DISABLE;
    lcd_init_struct.nwait_signal = DISABLE;
    lcd_init_struct.memory_write = ENABLE;
    lcd_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;
    lcd_init_struct.wrap_burst_mode = DISABLE;
    lcd_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;
    lcd_init_struct.burst_mode = DISABLE;
    lcd_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_16B;
    lcd_init_struct.memory_type = EXMC_MEMORY_TYPE_SRAM;
    lcd_init_struct.address_data_mux = DISABLE;
    lcd_init_struct.read_write_timing = &lcd_timing_init_struct;
    lcd_init_struct.write_timing = &lcd_timing_init_struct;

    exmc_norsram_init(&lcd_init_struct);

    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION0);
}
