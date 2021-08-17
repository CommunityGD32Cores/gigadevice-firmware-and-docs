/*!
    \file    main.c
    \brief   CRC calculate demo

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

#include "gd32f1x0.h"
#include <stdio.h>
#include "gd32f1x0r_eval.h"

uint32_t vab_32 = (uint32_t)0xabcd1234;
uint8_t  vba_8[4] = {0xab, 0xcd, 0x12, 0x34};
uint16_t vba_16[2] = {0xabcd, 0x1234};
uint32_t valcrc8_1, valcrc8_2, valcrc8_3, valcrc16_1, valcrc16_2, valcrc16_3, valcrc32_1, valcrc32_2, valcrc32_3;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    rcu_periph_clock_enable(RCU_CRC);
    
    /* CRC input data is byte */
    crc_deinit();
    valcrc8_1 = crc_block_data_calculate(vba_8, sizeof(vba_8)/sizeof(vba_8[0]), INPUT_FORMAT_BYTE);
    
    /* input reverse with byte */
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);
    valcrc8_2 = crc_block_data_calculate(vba_8, sizeof(vba_8)/sizeof(vba_8[0]), INPUT_FORMAT_BYTE);
    
    /* input reverse with byte and output reverse*/
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);
    crc_reverse_output_data_enable();
    valcrc8_3 = crc_block_data_calculate(vba_8, sizeof(vba_8)/sizeof(vba_8[0]), INPUT_FORMAT_BYTE);
    
    /* CRC input data is half word */
    crc_deinit();
    valcrc16_1 = crc_block_data_calculate(vba_16, sizeof(vba_16)/sizeof(vba_16[0]), INPUT_FORMAT_HALFWORD);
    
    /* input reverse with half word */
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_HALFWORD);
    valcrc16_2 = crc_block_data_calculate(vba_16, sizeof(vba_16)/sizeof(vba_16[0]), INPUT_FORMAT_HALFWORD);
    
    /* input reverse with half word and output reverse*/
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_HALFWORD);
    crc_reverse_output_data_enable();
    valcrc16_3 =crc_block_data_calculate(vba_16, sizeof(vba_16)/sizeof(vba_16[0]), INPUT_FORMAT_HALFWORD);
    
    /* CRC input data is word */
    crc_deinit();
    valcrc32_1 = crc_single_data_calculate(vab_32, INPUT_FORMAT_WORD);

    /* input reverse with word */
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_WORD);
    valcrc32_2 = crc_single_data_calculate(vab_32, INPUT_FORMAT_WORD);

    /* input reverse with word and output reverse*/
    crc_deinit();
    crc_input_data_reverse_config(CRC_INPUT_DATA_WORD);
    crc_reverse_output_data_enable();
    valcrc32_3 = crc_single_data_calculate(vab_32, INPUT_FORMAT_WORD);

    /* check the caculation valcrc */
    if((valcrc8_1==0xf7018a40) && (valcrc8_2==0x49fc6721) && (valcrc8_3==0x84E63F92)){
        gd_eval_led_on(LED1);
    }
    if((valcrc16_1==0xf7018a40) && (valcrc16_2==0x606444e3) && (valcrc16_3==0xc7222606)){
        gd_eval_led_on(LED2);
    }
    if((valcrc32_1==0xf7018a40) && (valcrc32_2==0x16d70081) && (valcrc32_3==0x8100eb68)){
        gd_eval_led_on(LED3);
    }

    while (1);
}
