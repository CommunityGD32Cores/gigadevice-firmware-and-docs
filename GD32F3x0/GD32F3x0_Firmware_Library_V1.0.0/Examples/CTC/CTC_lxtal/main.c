/*!
    \file  main.c
    \brief CTC is used to trim internal 48MHz RC oscillator with LXTAL clock
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "gd32f3x0_eval.h"

void ctc_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init led1*/
    gd_eval_led_init(LED1);

    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);
    /* wait till IRC48M is ready */
    rcu_osci_stab_wait(RCU_IRC48M);

    /* LXTAL clock config */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    /* enable LXTAL clock */
    rcu_osci_on(RCU_LXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);
    

    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);
    /* CTC config */
    ctc_config();

    while(1){
        /* if the clock trim is OK */
        if(ctc_flag_get(CTC_FLAG_CKOK) != RESET){
            gd_eval_led_on(LED1);
        }else{
            gd_eval_led_off(LED1);
        }
    }
}

/*!
    \brief      configure the CTC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_config(void)
{
    /* config CTC reference signal source prescaler */
    ctc_refsource_prescaler_config(CTC_REFSOURCE_PSC_OFF);
    /* select reference signal source */
    ctc_refsource_signal_select(CTC_REFSOURCE_LXTAL);
    /* select reference signal source polarity */
    ctc_refsource_polarity_config(CTC_REFSOURCE_POLARITY_RISING);
    /* config hardware automatically trim mode */
    ctc_hardware_trim_mode_config(CTC_HARDWARE_TRIM_MODE_ENABLE);
    
    /* config CTC counter reload value, Fclock/Fref-1 */
    ctc_counter_reload_value_config(0x05B8);
    /* config clock trim base limit value, Fclock/Fref*0.0012/2 */
    ctc_clock_limit_value_config(0x0002);

    /* CTC counter enable */
    ctc_counter_enable();
}
