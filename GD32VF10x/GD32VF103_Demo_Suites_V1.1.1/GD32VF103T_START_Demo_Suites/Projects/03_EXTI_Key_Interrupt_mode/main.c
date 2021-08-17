/*!
    \file    main.c
    \brief   EXTI key Interrupt demo

    \version 2019-09-18, V1.0.0, demo for GD32VF103
    \version 2020-12-18, V1.1.0, demo for GD32VF103
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

#include "gd32vf103.h"
#include "gd32vf103t_start.h"
#include "systick.h"

void led_init(void);
void led_flash(int times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{  
    led_init();

    /* enable the global interrupt */
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);

    led_flash(1);

     /* configure button pin as input */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    
    /* enable and set key EXTI interrupt to the lowest priority */
    eclic_irq_enable(EXTI0_IRQn, 1, 1);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0);

    /* configure key EXTI line */
    exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_0);
    
    while(1){
    }
}

/*!
    \brief      initialize the LED1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    /* enable the LED1 clock */
   rcu_periph_clock_enable(RCU_GPIOA);
   /* configure LED1 GPIO port */
   gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

   /* turn off LED1 */
   gpio_bit_reset(GPIOA, GPIO_PIN_8);
}

/*!
    \brief      flash the LED1 for test
    \param[in]  times: times to flash the LED1
    \param[out] none
    \retval     none
*/
void led_flash(int times)
{
    int i;
    for (i = 0;i < times;i++){
        /* delay 500 ms */
        delay_1ms(500);
        /* toggle the LED1 */
        gpio_bit_write(GPIOA, GPIO_PIN_8, (bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_8)));

        /* delay 500 ms */
        delay_1ms(500);
        /* toggle the LED1 */
        gpio_bit_write(GPIOA, GPIO_PIN_8, (bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_8)));
    }
}

