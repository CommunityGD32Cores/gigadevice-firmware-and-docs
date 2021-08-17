/*!
    \file  main.c
    \brief Deepsleep wakeup
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "gd32f3x0_it.h"
#include "gd32f3x0_eval.h"

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
    SysTick_Config((SystemCoreClock / 1000));
    
    gd_eval_led_init(LED2);
    
    /* USART configuration the CK_IRC8M as USART clock */
    rcu_usart_clock_config(RCU_USART0SRC_IRC8M);
    gd_eval_com_init(EVAL_COM1);

    nvic_irq_enable(USART0_IRQn, 0, 0);
    
    delay_s(20);
    
    {
        /* use start bit wakeup mcu */
        usart_wakeup_mode_config(EVAL_COM1, USART_WUM_STARTB);
        
        /* enable USART */
        usart_enable(EVAL_COM1);
        /* ensure USART is enabled */
        while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_REA));
        /* check USART is not transmitting */
        while(SET == usart_flag_get(EVAL_COM1, USART_FLAG_BSY));
        
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
        
        usart_receive_config(EVAL_COM1,USART_RECEIVE_ENABLE);
        
        while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TC));
        
        /* disable the USART */
        usart_disable(EVAL_COM1);
    }
    
    /* reconfigure systemclock */
    system_clock_reconfig();
    
    /* configure and enable the systick timer to generate an interrupt each 1 ms */
    SysTick_Config((SystemCoreClock / 1000));
    
    while (1);
}

/*!
    \brief      delay function
    \param[in]  nTime
    \param[out] none
    \retval     none
*/
void delay_s(uint32_t nTime)
{
    uint32_t TimingDelay = 10800000*nTime;
    while(TimingDelay != 0)
        TimingDelay--;
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
    RCU_CTL0 |= RCU_CTL0_HXTALEN;
    
    HSEStatus = rcu_osci_stab_wait(RCU_HXTAL);
    if (SUCCESS == HSEStatus){
        /* configure AHB */
        rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1); 
        
        /* configure APB1, APB2 */
        rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
        rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
            
        /*  PLL configuration: = HXTAL/2 * 27 = 108 MHz */
        rcu_hxtal_prediv_config(RCU_PLL_PREDV2);
        rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL27);
        
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
void LED_Spark(void)
{
    static __IO uint32_t TimingDelayLocal = 0;
    
    if (0x00 != TimingDelayLocal){
        if(TimingDelayLocal < 50){
            /* light on */
            gd_eval_led_on(LED2);
        }else{
            /* light off */
            gd_eval_led_off(LED2);
        }
        TimingDelayLocal--;
    }else{
        TimingDelayLocal = 100;
    }
}
