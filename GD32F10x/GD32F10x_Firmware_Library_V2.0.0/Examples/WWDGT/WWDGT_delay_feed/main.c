/*!
    \file  main.c
    \brief WWDGT delay feed demo 
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#include "gd32f10x.h"
#include "systick.h"
#include "gd32f10x_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* configure LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    
    /* turn off LED1 and LED2 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    
    /* delay */
    delay_1ms(150);

    /* check if the system has resumed from WWDGT reset */
    if(RESET != rcu_flag_get(RCU_FLAG_WWDGTRST)){
        /* WWDGTRST flag set */
        gd_eval_led_on(LED1);
        /* clear the WWDGTRST flag */
        rcu_all_reset_flag_clear();
        while(1){
        }
    }
    /* enable WWDGT clock */
    rcu_periph_clock_enable(RCU_WWDGT);

    /*
     *  set WWDGT clock = (PCLK1 (54MHz)/4096)/8 = 1648Hz (~606.8 us)
     *  set counter value to 127
     *  set window value to 80
     *  refresh window is: ~606.8 * (127-80)= 28.5ms < refresh window < ~606.8 * (127-63) =38.8ms.
     */
    wwdgt_config(127, 80, WWDGT_CFG_PSC_DIV8);
    wwdgt_enable();

    while(1){
        /* toggle LED2 */
        gd_eval_led_toggle(LED2);
        /* insert 30 ms delay */
        delay_1ms(30);
        /* update WWDGT counter */
        wwdgt_counter_update(127);
    }
}
