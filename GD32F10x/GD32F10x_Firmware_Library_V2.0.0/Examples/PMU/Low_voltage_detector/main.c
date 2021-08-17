/*!
    \file  main.c
    \brief low voltage detector
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "gd32f10x.h"
#include <stdio.h>
#include "gd32f10x_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure NVIC */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
    nvic_irq_enable(LVD_IRQn,0,0);
    /* clock enable */
    rcu_periph_clock_enable(RCU_PMU);
    /* initialize LED1 */
    gd_eval_led_init(LED1);
    /* turn on LED1*/
    gd_eval_led_on(LED1);
    /* configure EXTI_16  */
    exti_init(EXTI_16, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    /* configure the lvd threshold to 2.9v */
    pmu_lvd_select(PMU_LVDT_7);

    while(1){
    }
}
