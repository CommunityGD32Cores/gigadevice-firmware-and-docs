/*!
    \file    main.c
    \brief   Deepsleep wakeup
    
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
#include "gd32f1x0_it.h"
#include "gd32f1x0r_eval.h"
#include "systick.h"

extern __IO uint8_t counter0;

static void system_clock_reconfig(void);
void delay_s(uint32_t nTime);

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
    
    /* USART configuration the CK_IRC8M as USART clock */
    rcu_usart_clock_config(RCU_USART0SRC_IRC8M);
    
#ifdef GD32F130_150
    gd_eval_com_init(EVAL_COM0);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    
    delay_1ms(5000);
    
    {
        /* use start bit wakeup mcu */
        usart_wakeup_mode_config(EVAL_COM0, USART_WUM_STARTB);
        
        /* enable USART */ 
        usart_enable(EVAL_COM0);
        
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_REA));
        
        usart_wakeup_enable(EVAL_COM0);
        
        /* enable the WUIE interrupt */ 
        usart_interrupt_enable(EVAL_COM0, USART_INT_WU);

        /* enable PWU APB clock */
        rcu_periph_clock_enable(RCU_PMU);
            
        /* enter deep-sleep mode */
        pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
        
        /* wait a WUIE interrup event */
        while(0x00 == counter0);
        
        /* disable USART peripheral in deepsleep mode */ 
        usart_wakeup_disable(EVAL_COM0);
        
        while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE));
        usart_data_receive(EVAL_COM0);
        
        usart_receive_config(EVAL_COM0, USART_RECEIVE_ENABLE);
        
        while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TC));
        
        /* disable the USART */
        usart_disable(EVAL_COM0);
    }
#elif defined GD32F170_190
    gd_eval_com_init(EVAL_COM1);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    
    delay_1ms(5000);
    
    {
        /* use start bit wakeup mcu */
        usart_wakeup_mode_config(EVAL_COM1, USART_WUM_STARTB);
        
        /* enable USART */ 
        usart_enable(EVAL_COM1);
        
        while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_REA));
        
        usart_wakeup_enable(EVAL_COM1);
        
        /* enable the WUIE interrupt */ 
        usart_interrupt_enable(EVAL_COM1, USART_INT_WU);

        /* enable PWU APB clock */
        rcu_periph_clock_enable(RCU_PMU);
            
        /* enter deep-sleep mode */
        pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
        
        /* wait a WUIE interrup event */
        while(0x00 == counter0);
        
        /* disable USART peripheral in deepsleep mode */ 
        usart_wakeup_disable(EVAL_COM1);
        
        while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_RBNE));
        usart_data_receive(EVAL_COM1);
        
        usart_receive_config(EVAL_COM1, USART_RECEIVE_ENABLE);
        
        while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TC));
        
        /* disable the USART */
        usart_disable(EVAL_COM1);
    }
#endif /* GD32F130_150 */
    
    /* reconfigure systemclock */
    system_clock_reconfig();
    
    /* configure and enable the systick timer to generate an interrupt each 1 ms */
    SysTick_Config((SystemCoreClock / 1000));
    
    while (1);
}

/*!
    \brief      restore peripheral config before entering deepsleep mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_reconfig(void)
{
    __IO uint32_t StartUpCounter = 0, HXTALStatus = 0;
    
    /* enable HXTAL */
    RCU_CTL0 |= RCU_CTL0_HXTALEN;
    
    HXTALStatus = rcu_osci_stab_wait(RCU_HXTAL);
    if (SUCCESS == HXTALStatus){
        /* configure AHB */
        rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1); 
        
        /* configure APB1, APB2 */
        rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
        rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
            
        /* PLL configuration: = HXTAL * 9 = 72 MHz */
        rcu_hxtal_prediv_config(RCU_PLL_HXTAL_DIV1);
        rcu_pll_config(RCU_PLLSRC_HXTAL, RCU_PLL_MUL9);
        
        /* enable PLL */
        RCU_CTL0 |= RCU_CTL0_PLLEN;
        
        /* select PLL as system clock */
        RCU_CFG0 &= ~RCU_CFG0_SCS;
        RCU_CFG0 |= RCU_CKSYSSRC_PLL;
    } 
}

/*!
    \brief      LED spark
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t TimingDelayLocal = 0;
    
    if (0x00 != TimingDelayLocal){
        if(TimingDelayLocal < 1000){
            /* light on */
            gd_eval_led_on(LED2);
        }else{
            /* light off */
            gd_eval_led_off(LED2);
        }
        TimingDelayLocal--;
    }else{
        TimingDelayLocal = 2000;
    }
}
