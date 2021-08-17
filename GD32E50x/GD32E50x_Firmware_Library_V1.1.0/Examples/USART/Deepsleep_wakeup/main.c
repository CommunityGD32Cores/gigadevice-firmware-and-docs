/*!
    \file    main.c
    \brief   Deepsleep wakeup

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

#include "gd32e50x.h"
#include <stdio.h>
#include "gd32e507z_eval.h"
#include "systick.h"

extern __IO uint8_t counter0;

static void system_clock_reconfig(void);
void usart5_config(void);
    
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

    /* USART configuration the CK_IRC8M as USART5 clock */
    rcu_usart5_clock_config(RCU_USART5SRC_IRC8M);
    /* configure USART5 */
    usart5_config();

    nvic_irq_enable(USART5_WKUP_IRQn, 0, 0);

    /* USART5 Wakeup EXTI line configuretion */
    exti_init(EXTI_21, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    delay_1ms(3000);
    
    {
        /* use start bit wakeup mcu */
        usart5_wakeup_mode_config(USART5, USART5_WUM_STARTB);
        /* enable USART5 */
        usart_enable(USART5);
        /* ensure USART5 is enabled */
        while(RESET == usart5_flag_get(USART5, USART5_FLAG_REA));
        /* check USART5 is not transmitting */
        while(SET == usart5_flag_get(USART5, USART5_FLAG_BSY));

        usart5_wakeup_enable(USART5);
        /* enable the WUIE interrupt */
        usart5_interrupt_enable(USART5, USART5_INT_WU);

        /* enable PMU APB clock */
        rcu_periph_clock_enable(RCU_PMU);
        /* enter deep-sleep mode */
        pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);

        /* wait a WUIE interrup event */
        while(0x00 == counter0);
        
        /* disable USART5 peripheral in deepsleep mode */
        usart5_wakeup_disable(USART5);

        while(RESET == usart5_flag_get(USART5, USART5_FLAG_RBNE));
        usart_data_receive(USART5);

        usart_receive_config(USART5, USART_RECEIVE_ENABLE);
        
        while (RESET == usart5_flag_get(USART5, USART5_FLAG_TC));
        
        /* disable the USART */
        usart_disable(USART5);
    }

    /* reconfigure systemclock */
    system_clock_reconfig();

    systick_config();

    while(1);
}


/*!
    \brief      restore peripheral config before entering deepsleep mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void system_clock_reconfig(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    HSEStatus = rcu_osci_stab_wait(RCU_HXTAL);
    if (SUCCESS == HSEStatus){
        /* configure AHB */
        rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1); 

        /* configure APB1, APB2 */
        rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
        rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);


#if (defined(GD32E50X_HD) || defined(GD32E50X_XD) || defined(GD32EPRT))
        /*  PLL configuration: = HXTAL/2 *45 = 180 MHz */
        rcu_pllpresel_config(RCU_PLLPRESRC_HXTAL);
        rcu_predv0_config(RCU_PREDV0_DIV2);
        rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL45);
#elif defined (GD32E50X_CL)
        /*  PLL configuration: = HXTAL/5 * 8 / 10 *45 = 180 MHz */
        rcu_pllpresel_config(RCU_PLLPRESRC_HXTAL);
        rcu_predv0_config(RCU_PREDV0SRC_HXTAL_IRC48M, RCU_PREDV0_DIV5);
        rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL36);
#endif

        /* enable PLL */
        RCU_CTL |= RCU_CTL_PLLEN;
        
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
void LED_Spark(void)
{
    static __IO uint32_t TimingDelayLocal = 0;

    if (0x00 != TimingDelayLocal){
        if(TimingDelayLocal < 500){
            /* light on */
            gd_eval_led_on(LED2);
        }else{
            /* light off */
            gd_eval_led_off(LED2);
        }
        TimingDelayLocal--;
    }else{
        TimingDelayLocal = 1000;
    }
}

/*!
    \brief      configure USART5
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart5_config(void)
{
    /* enable USART5 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable AF clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable USART5 clock */
    rcu_periph_clock_enable(RCU_USART5);

    /* connect port to USART5_Tx */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* connect port to USART5_Rx */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* configure USART5 Tx Rx as alternate function */
    gpio_afio_port_config(AFIO_PC6_USART5_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PC7_USART5_CFG, ENABLE);

    /* USART5 configure */
    usart_deinit(USART5);
    usart_baudrate_set(USART5, 115200U);
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);

    usart_enable(USART5);
}
