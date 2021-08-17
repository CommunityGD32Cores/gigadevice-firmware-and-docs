/*!
    \file  main.c
    \brief enet demo 

    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx.h"
#include "netconf.h"
#include "main.h"
#include "lwip/tcp.h"
#include "lwip/lwip_timers.h"
#include "gd32f450z_eval.h"
#include "httpd.h"

#define SYSTEMTICK_PERIOD_MS  10

__IO uint32_t g_localtime = 0; /* for creating a time reference incremented by 10ms */
uint32_t g_timedelay;

void adc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM0);
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
  
    /* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */ 
    enet_system_setup();

    adc_config();
  
    /* initilaize the LwIP stack */
    lwip_stack_init();

    /* initilaize the webserver */
    httpd_init();
  
    while(1){
      
#ifndef USE_ENET_INTERRUPT      
        /* check if any packet received */
        if(enet_rxframe_size_get()){ 
            /* process received ethernet packet */
            lwip_pkt_handle();
        }        
#endif /* USE_ENET_INTERRUPT */
        
        /* handle periodic timers for LwIP */
#ifdef TIMEOUT_CHECK_USE_LWIP
        sys_check_timeouts();
        
#ifdef USE_DHCP
        lwip_dhcp_process_handle();
#endif /* USE_DHCP */
        
#else
        lwip_periodic_handle(g_localtime);
#endif /* TIMEOUT_CHECK_USE_LWIP */
    }   
}

/*!
    \brief      insert a delay time
    \param[in]  ncount: number of 10ms periods to wait for
    \param[out] none
    \retval     none
*/
void delay_10ms(uint32_t ncount)
{
    /* capture the current local time */
    g_timedelay = g_localtime + ncount;  

    /* wait until the desired delay finish */  
    while(g_timedelay > g_localtime){
    }
}

/*!
    \brief      updates the system local time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_update(void)
{
    g_localtime += SYSTEMTICK_PERIOD_MS;
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_HCLK_DIV5);

    /* ADC channel length config */
    adc_channel_length_config(ADC0,ADC_INSERTED_CHANNEL,1);

    /* ADC Vref channel config */
    adc_inserted_channel_config(ADC0,0,ADC_CHANNEL_17,ADC_SAMPLETIME_144);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0,ADC_INSERTED_CHANNEL,DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    /* ADC SCAN function enable */
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);
    adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);
    adc_special_function_config(ADC0,ADC_INSERTED_CHANNEL_AUTO,ENABLE);  
    /* ADC Vbat channel enable */
    adc_channel_16_to_18(ADC_VBAT_CHANNEL_SWITCH,ENABLE);  
    /* ADC temperature and Vref enable */
    adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);
    
    /* enable ADC interface */
    adc_enable(ADC0);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0,ADC_INSERTED_CHANNEL);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
