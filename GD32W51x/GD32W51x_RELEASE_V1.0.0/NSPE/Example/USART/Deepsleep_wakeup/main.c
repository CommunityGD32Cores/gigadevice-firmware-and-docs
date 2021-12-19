/*!
    \file    main.c
    \brief   Deepsleep wakeup
    
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
#include "systick.h"

extern __IO uint8_t counter0;
static void system_clock_reconfig(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{    
    systick_config();
    gd_eval_led_init(LED2);
    
    /* configurate the CK_IRC16M as USART clock */
    rcu_usart2_clock_config(RCU_USART2SRC_IRC16M);
    gd_eval_com_init(EVAL_COM0);

    nvic_irq_enable(USART2_IRQn, 0, 0);
    /* USART wakeup EXTI line configuretion */
    exti_init(EXTI_28, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    
    delay_1ms(2000);
    
    /* use start bit wakeup MCU */
    usart_wakeup_mode_config(EVAL_COM0, USART_WUM_STARTB);
    
    /* enable USART */
    usart_enable(EVAL_COM0);
    /* ensure USART is enabled */
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_REA)){
    }
    
    /* check USART is not transmitting */
    while(SET == usart_flag_get(EVAL_COM0, USART_FLAG_BSY)){
    }
    
    usart_wakeup_enable(EVAL_COM0);
    /* enable the WUIE interrupt */
    usart_interrupt_enable(EVAL_COM0, USART_INT_WU);

    /* enable PWU APB clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enter deep-sleep mode */
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);

    /* wait a WUIE interrupt event */
    while(0x00 == counter0){
    }
    
    /* reconfigurate system clock */
    system_clock_reconfig();
    
    systick_config();
    
    /* disable USART peripheral in deepsleep mode */
    usart_wakeup_disable(EVAL_COM0);
    
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE)){
    }
    
    usart_data_receive(EVAL_COM0);
    
    usart_receive_config(EVAL_COM0, USART_RECEIVE_ENABLE);
    
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC)){
    }
    
    /* disable the USART */
    usart_disable(EVAL_COM0);

    while(1){
    }
}

/*!
    \brief      configurate the system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_reconfig(void)
{
    /* power up HXTAL */
    RCU_CTL |= RCU_CTL_HXTALPU;
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;
    RCU_CTL |= RCU_CTL_HXTALENPLL;

    RCU_CTL |= RCU_CTL_HXTALREADY;
    while(ERROR == rcu_osci_stab_wait(RCU_HXTAL));

    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL0 |= PMU_CTL0_LDOVS;

    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1); 
    
    /* configurate APB1, APB2 */
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV4);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV2);
        
    /* configurate the main PLL, PSC = 25, PLL_N = 360, PLL_P = 2 */
    rcu_pll_config(RCU_PLLSRC_HXTAL, 40, 360, 2);
    
    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)){
    }
}

/*!
    \brief      led spark
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timing_delay = 0;
    
    if(0x00 != timing_delay){
        if(timing_delay < 500){
            /* light on */
            gd_eval_led_on(LED2);
        }else{
            /* light off */
            gd_eval_led_off(LED2);
        }
        timing_delay--;
    }else{
        timing_delay = 1000;
    }
}
