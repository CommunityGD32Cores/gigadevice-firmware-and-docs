/*!
    \file    main.c
    \brief   TMU mode6 calculate example

    \version 2020-03-02, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include "gd32e507z_eval.h"

uint32_t input_val_temp0, input_val_temp1, output_val_temp0, output_val_temp1;
float input_val0, input_val1;
float output_val0, output_val1;

void led_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    input_val0 = -1.0;
    input_val1 = 1.0;
    /* change the input_val to meet IEEE 32-Bit Single Precision Floating-Point Format */
    input_val_temp0 = *((uint32_t *)(&input_val0));
    input_val_temp1 = *((uint32_t *)(&input_val1));
    led_config();

    /* enable TMU clock */
    rcu_periph_clock_enable(RCU_TMU);
    /* reset TMU */
    tmu_deinit();
    /* configure TMU use mode0*/
    tmu_mode_set(TMU_MODE6);
    /* enable the TMU interrupt*/
    tmu_interrupt_enable();
    /* write the value to input data0 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    tmu_idata0_write(input_val_temp0);
    /* write the value to input data1 register */
    /* the vlaue must meet IEEE 32-Bit Single Precision Floating-Point Format */
    tmu_idata1_write(input_val_temp1);
    /* start the TMU to calculation */
    tmu_enable();
    /* wait for the calculation finished */
    while(!tmu_interrupt_flag_get(TMU_INT_FLAG_CFIF));
    /* check the TMU overflow flag*/
    if((SET == tmu_flag_get(TMU_FLAG_OVRF)) || (SET == tmu_flag_get(TMU_FLAG_UDRF))){
        gd_eval_led_on(LED3);
    }else{
        /* read the TMU calculation result */
        /* the result value meet IEEE 32-Bit Single Precision Floating-Point Format */
        output_val_temp0 = tmu_data0_read();
        /* read the TMU calculation result */
        /* the result value meet IEEE 32-Bit Single Precision Floating-Point Format */
        output_val_temp1 = tmu_data1_read();
        output_val0 = *((float *)(&output_val_temp0));
        output_val1 = *((float *)(&output_val_temp1));
        gd_eval_led_on(LED2);
    }
    while(1){
    }
}

/*!
    \brief      configure LED
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_config(void)
{  
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* LED off */
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
}
