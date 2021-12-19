/*!
    \file    main.c
    \brief   ADC channel of temperature and Vref demo

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "gd32l23x.h"
#include "systick.h"
#include <stdio.h>
#include "gd32l233r_eval.h"

#define ADC_TEMP_CALIBRATION_VALUE          REG16(0x1FFFF7F8)

float temperature;
float vref_value;

void rcu_config(void);
void adc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();
    /* system clocks configuration */
    rcu_config();
    /* ADC configuration */
    adc_config();
    /* USART configuration */
    gd_eval_com_init(EVAL_COM);

    while(1) {
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC_INSERTED_CHANNEL);
        /* delay a time in milliseconds */
        delay_1ms(2000U);

        /* value convert */
        temperature = ((float)((int32_t)ADC_IDATA0 - (int32_t)ADC_TEMP_CALIBRATION_VALUE) * 3.3f / 4095 * 1000 / 3.3f) + 30;
        vref_value = (ADC_IDATA1 * 3.3f / 4095);

        /* value print */
        printf(" the Temperature data is %2.0f degrees Celsius\r\n", temperature);
        printf(" the Reference voltage data is %5.3fV \r\n", vref_value);
        printf(" \r\n");
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV16);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 2);

    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(0U, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
    /* ADC internal reference voltage channel config */
    adc_inserted_channel_config(1U, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC SCAN function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC temperature and Vrefint enable */
    adc_channel_16_to_19(ADC_TEMP_CHANNEL_SWITCH, ENABLE);
    adc_channel_16_to_19(ADC_INTERNAL_CHANNEL_SWITCH, ENABLE);

    adc_external_trigger_config(ADC_INSERTED_CHANNEL, ENABLE);

    /* 64 times sample, 6 bits shift */
    adc_oversample_mode_config(ADC_OVERSAMPLING_ALL_CONVERT, ADC_OVERSAMPLING_SHIFT_8B, ADC_OVERSAMPLING_RATIO_MUL256);
    adc_oversample_mode_enable();

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);

    /* ADC calibration and reset calibration */
    adc_calibration_enable();
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));

    return ch;
}
