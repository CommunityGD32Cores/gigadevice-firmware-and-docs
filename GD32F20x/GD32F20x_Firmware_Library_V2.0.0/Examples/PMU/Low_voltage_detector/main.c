/*!
    \file  main.c
    \brief low voltage detector
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#include "gd32f20x.h"
#include <stdio.h>
#include "gd32f207i_eval.h"

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
    /* configure the lvd threshold to 3.1v */
    pmu_lvd_select(PMU_LVDT_7);

    while(1){
    }
}
