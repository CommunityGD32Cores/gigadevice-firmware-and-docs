/*!
    \file    main.c
    \brief   ADC channel of temperature, Vref

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
#include "systick.h"
#include <stdio.h>
#include "gd32f1x0r_eval.h"

#ifdef GD32F130_150
float adc_reference_voltage = 3.3f;
#elif defined GD32F170_190
float adc_reference_voltage = 5.0f;
#endif /* GD32F130_150 */

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
#ifdef GD32F130_150
    gd_eval_com_init(EVAL_COM0);
#elif defined GD32F170_190
    gd_eval_com_init(EVAL_COM1);
#endif /* GD32F130_150 */

    while(1){
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC_INSERTED_CHANNEL);
        /* delay a time in milliseconds */
        delay_1ms(2000U);

        /* value convert */
        temperature = (1.43f - ADC_IDATA0 * adc_reference_voltage/4096) * 1000 / 4.3f + 30;
        vref_value = (ADC_IDATA1 * adc_reference_voltage / 4096);

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
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
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
    adc_channel_length_config(ADC_INSERTED_CHANNEL,2U);

    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(0U, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
    /* ADC internal reference voltage channel config */
    adc_inserted_channel_config(1U, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_INSERTED_CHANNEL, ENABLE);
    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC SCAN function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC temperature and Vrefint enable */
    adc_tempsensor_vrefint_enable();
    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);
    
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
#ifdef GD32F130_150
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
#elif defined GD32F170_190
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));
    return ch;
#endif /* GD32F130_150 */
}
