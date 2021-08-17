/*!
    \file  main.c
    \brief CRC calculate demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f3x0_eval.h"

uint32_t vab1 = 0, success_flag = 0;
uint32_t read32_1, read32_2, read32_3, read32_4, read32_5, read32_6, read32_7, read32_8;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);

    vab1 = (uint32_t)0xabcd1234;
    rcu_periph_clock_enable(RCU_CRC);
    
    crc_deinit();
    read32_1 = crc_single_data_calculate(vab1);

    /* input reverse */
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);
    read32_2 = crc_single_data_calculate(vab1);

    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_HALFWORD);
    read32_3 = crc_single_data_calculate(vab1);

    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_WORD);
    read32_4 = crc_single_data_calculate(vab1);

    /* output reverse */
    crc_deinit();
    crc_reverse_output_data_enable();
    read32_5 = crc_single_data_calculate(vab1);

    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);
    crc_reverse_output_data_enable();
    read32_6 = crc_single_data_calculate(vab1);

    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_HALFWORD);
    crc_reverse_output_data_enable();
    read32_7 = crc_single_data_calculate(vab1);

    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_WORD);
    crc_reverse_output_data_enable();
    read32_8 = crc_single_data_calculate(vab1);

    /* check the caculation result */
    if((read32_1 == 0xf7018a40U)&&(read32_2 == 0x49fc6721U)&&(read32_3 == 0x606444e3U)&&(read32_4 == 0x16d70081U)
        &&(read32_5 == 0x025180efU)&&(read32_6 == 0x84e63f92U)&&(read32_7 == 0xc7222606U)&&(read32_8 == 0x8100eb68U)){
        success_flag = 0x1U;
        gd_eval_led_on(LED1);
    }

    while (1);
}
