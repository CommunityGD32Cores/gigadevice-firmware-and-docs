/*!
    \file  main.c
    \brief ADC channel of temperature and Vref demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f3x0_eval.h"

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
    gd_eval_com_init(EVAL_COM1);

    while(1){
        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC_INSERTED_CHANNEL);
        /* delay a time in milliseconds */
        delay_1ms(2000);

        /* value convert */
        temperature = (1.43 - ADC_IDATA0 * 3.3/4096) * 1000 / 4.3 + 25;
        vref_value = (ADC_IDATA1 * 3.3 / 4096);

        /* value print */
        printf(" the temperature data is %2.0f degrees Celsius\r\n", temperature);
        printf(" the reference voltage data is %5.3fV \r\n", vref_value);
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
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 2);

    /* ADC temperature sensor channel config */
    adc_inserted_channel_config(0, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
    /* ADC internal reference voltage channel config */
    adc_inserted_channel_config(1, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC SCAN function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC temperature and Vrefint enable */
    adc_tempsensor_vrefint_enable();

    adc_external_trigger_config(ADC_INSERTED_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1);
    
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}
