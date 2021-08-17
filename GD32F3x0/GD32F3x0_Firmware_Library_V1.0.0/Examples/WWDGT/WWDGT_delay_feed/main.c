/*!
    \file  main.c
    \brief wwdgt delay feed demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
#include "gd32f3x0_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    gd_eval_led_on(LED1);
    gd_eval_led_on(LED2);

    systick_config();

    /* check if the system has resumed from WWDGT reset */
    if (RESET != rcu_flag_get(RCU_FLAG_WWDGTRST)){   
        /* WWDGTRST flag set */
        gd_eval_led_on(LED1);
        rcu_all_reset_flag_clear();
        while(1);
    }else{
        delay_1ms(150);
        gd_eval_led_off(LED1);
    }

    /* enable WWDGT clock */
    rcu_periph_clock_enable(RCU_WWDGT);
    
    /* enable WWDGT and set counter value to 127, WWDGT timeout = ~607 us * 64 = 38.8 ms.
       in this case the refresh window is: ~607 * (127-80)= 28.5ms < refresh window < ~607 * 64 =38.8ms.
       set window value to 80; WWDGT counter should be refreshed only when the counter
       is below 80 (and greater than 64) otherwise a reset will be generated.
       WWDGT clock counter = (PCLK1 (54MHz)/4096)/8 = 1647Hz (~607 us) */
    wwdgt_config(127,80,WWDGT_CFG_PSC_DIV8);
    wwdgt_enable();

    while (1){
        gd_eval_led_toggle(LED2);
        /* insert 30 ms delay */
        delay_1ms(30);

        /* update WWDGT counter */
        wwdgt_counter_update(127);
    }
}
