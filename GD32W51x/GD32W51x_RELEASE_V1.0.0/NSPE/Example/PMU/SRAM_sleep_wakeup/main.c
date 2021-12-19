/*!
    \file    main.c
    \brief   SRAM3 go to sleep and wakeup

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

#include "gd32w51x.h"
#include <stdio.h>
#include "gd32w515p_eval.h"

#define SRAM3_START_ADDR    SRAM3_BASE

uint8_t data_array[10]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

void led_config(void);
void sram_write_check(void);
void sram_wakeup_check(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{   
    /* led configuration */
    led_config();
    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);

    /* write data to SRAM3 and check */    
    sram_write_check();    

    /* TAMPER_WAKEUP key config */
    gd_eval_key_init(KEY_TAMPER_WAKEUP, KEY_MODE_GPIO);

    /* SRAM3 enter sleep mode */ 
    pmu_wifi_sram_control(PMU_SRAM3_SLEEP);
    /* turn on LED3 */
    gd_eval_led_on(LED3);    

    while(1){
        /* press TAMPER_WAKEUP key to wakeup SRAM3 */
        if(RESET == gpio_input_bit_get(TAMPER_WAKEUP_KEY_GPIO_PORT, TAMPER_WAKEUP_KEY_PIN)){
            pmu_wifi_sram_control(PMU_SRAM3_WAKE);
            /* check SRAM3 after wakeup from SRAM sleep */
            sram_wakeup_check();
        }
    }
}

/*!
    \brief      write data to SRAM3 and check
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sram_write_check(void)
{
    uint8_t data_i = 0;
    uint32_t write_p = 0;

    write_p = SRAM3_START_ADDR;
    /* write data array to SRAM3 */
    for(data_i = 0; data_i < 10; data_i++){
        (*(volatile uint8_t *)(uint32_t)(write_p)) = data_array[data_i];
        /* check whether data is right written to SRAM3 or not */
        if((*(volatile uint8_t *)(uint32_t)(write_p)) != data_array[data_i]){
            /* turn on LED2 to indicate error written occurs */
            gd_eval_led_on(LED2);
            while(1);
        }
        write_p++;
    }
} 

/*!
    \brief      check SRAM3 after wakeup from SRAM sleep
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sram_wakeup_check(void)
{
    uint8_t data_i = 0;
    volatile uint32_t write_p = 0, lost_state = 0;
    
    write_p = SRAM3_START_ADDR;
    /* check whether data in SRAM3 are lost or not */
    for(data_i = 0; data_i < 10; data_i++){    
        if((*(volatile uint8_t *)(uint32_t)(write_p)) != data_array[data_i]){
            lost_state++;
        }
        write_p++;
    }
    /* if data are all not lost, it is error situation */
    if(0 == lost_state){
        gd_eval_led_on(LED2);
    }else{
        /* if data are lost, it is right situation */
        gd_eval_led_on(LED1);
    }
} 

/*!
    \brief      LEDs configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);    
}
