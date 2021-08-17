/*!
    \file  main.c
    \brief DAC demo

    \version 2016-01-15, V1.0.0, demo for GD32F1x0
    \version 2016-05-13, V2.0.0, demo for GD32F1x0
    \version 2019-11-20, V3.0.0, demo for GD32F1x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

/*!
    \brief      GPIO configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOA);
          
    /* configure PA4 (dac output ) as analog output */
    gpio_mode_set(GPIOA,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_PIN_4);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4);
}

/*!
    \brief      DAC configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    /* enable the clock of DAC */
    rcu_periph_clock_enable(RCU_DAC);

    /* set DAC trigger source */
    dac0_trigger_source_config(DAC_TRIGGER_SOFTWARE);
    /* disable output buffer */
    dac0_output_buffer_disable();
    /* enable DAC */
    dac0_enable();
    /* set data left 12b alignment */
    dac0_data_set(DAC_ALIGN_12B_L, 0x7FF0);
    /* enable DAC software trigger */
    dac0_software_trigger_enable();
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* GPIO configuration */
    gpio_config();
    /* DAC configuration */
    dac_config();
    /* infinite loop */
    while (1);
}
