/*!
    \file    main.c
    \brief   CRC calculate example

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include "gd32e507z_eval.h"

#define SIZE_ARRAY(a)          (sizeof(a)/sizeof(a[0]))

uint32_t valcrc0 = 0, valcrc1=0, valcrc2 = 0, valcrc3 = 0, valcrc4 = 0;
uint32_t val_32[4] = {0xabcd1234, 0x3456cdef, 0x789a1524, 0x12346792};
uint16_t val_16[8] = {0xabcd, 0x1234, 0x3456, 0xcdef, 0x789a, 0x1524, 0x1234, 0x6792};
uint8_t val_8[16] = {0xab, 0xcd, 0x12, 0x34, 0x34, 0x56, 0xcd, 0xef, 0x78, 0x9a, 0x15, 0x24, 0x12, 0x34, 0x67, 0x92};
uint8_t val_7[16] = {0xab, 0xcd, 0x12, 0x34, 0x34, 0x56, 0xcd, 0xef, 0x78, 0x9a, 0x15, 0x24, 0x12, 0x34, 0x67, 0x92};

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize LEDs */
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* turn off LEDs */
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);

    /* enable CRC clock */
    rcu_periph_clock_enable(RCU_CRC);

    /* 1 : calculate the 32-bit CRC value of a 32-bit data */
    /* deinit CRC calculation unit */
    crc_deinit();

    /* calculate the CRC value */
    valcrc0 = crc_single_data_calculate(val_32[0], INPUT_FORMAT_WORD);

    /* 2 : calculate the 32-bit CRC value of a 32-bit data array */
    /* deinit CRC calculation unit */
    crc_deinit();

    /* calculate the CRC value */
    valcrc1 = crc_block_data_calculate(val_32, SIZE_ARRAY(val_32),INPUT_FORMAT_WORD);

    /* 3 : modify polynomial, configure input and output data reverse function 
       and calculate the 16-bit CRC value of a 16-bit data array */
    /* deinit CRC calculation unit */
    crc_deinit();

    /* configure the size of polynomial */
    crc_polynomial_size_set(CRC_CTL_PS_16);

    /* configure polynomial value */
    crc_polynomial_set(0x8320);

    /* configure the CRC input data reverse function */
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);

    /* enable the reverse operation of output data */
    crc_reverse_output_data_enable();

    /* calculate the CRC value */
    valcrc2 = crc_block_data_calculate((uint32_t *)val_16, SIZE_ARRAY(val_16), INPUT_FORMAT_HALFWORD);

    /* 4 : modify polynomial, configure input data reverse function 
       and calculate the 8-bit CRC value of a 8-bit data array */
    /* deinit CRC calculation unit */
    crc_deinit();

    /* configure the size of polynomial */
    crc_polynomial_size_set(CRC_CTL_PS_8);

    /* configure polynomial value */
    crc_polynomial_set(0x82);

    /* configure the CRC input data reverse function */
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);

    /* calculate the CRC value */
    valcrc3 = crc_block_data_calculate((uint32_t *)val_8, SIZE_ARRAY(val_8), INPUT_FORMAT_BYTE);

    /* 5 : modify polynomial, configure input data reverse function 
       and calculate the 7-bit CRC value of a 8-bit data array */
    /* deinit CRC calculation unit */
    crc_deinit();

    /* configure the size of polynomial */
    crc_polynomial_size_set(CRC_CTL_PS_7);

    /* configure polynomial value */
    crc_polynomial_set(0x74);

    /* configure the CRC input data reverse function */
    crc_input_data_reverse_config(CRC_INPUT_DATA_BYTE);

    /* calculate the CRC value */
    valcrc4 = crc_block_data_calculate((uint32_t *)val_7, SIZE_ARRAY(val_7), INPUT_FORMAT_BYTE);

    /* judge the result of CRC calculate */
    if((0xF7018A40 == valcrc0) && (0x16BAEEDE == valcrc1) \
        && (0x039B == valcrc2) && (0x24 == valcrc3) && (0x28 == valcrc4)){
        /* success */
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
    }

    while(1){
    }
}
