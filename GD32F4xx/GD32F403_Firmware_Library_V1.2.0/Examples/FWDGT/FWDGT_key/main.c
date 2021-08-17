/*!
    \file  main.c
    \brief FWDGT key demo 
*/
/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"
#include "systick.h"
#include "gd32f403_eval.h"


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    
    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){
    }
    
    /* config systick  */
    systick_config();
    
    /* configure LED  */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    
    /* configure the Tamper key which is used to reload FWDGT  */
    gd_eval_key_init(KEY_TAMPER,KEY_MODE_EXTI);
    
    delay_1ms(500);
    
    /* confiure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    fwdgt_config(2*500,FWDGT_PSC_DIV64);
    
    /* After 1.6 seconds to generate a reset */
    fwdgt_enable();

    /* check if the system has resumed from FWDGT reset */
    if (RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)){
        /* turn on LED2 */
        gd_eval_led_on(LED2);
        /* clear the FWDGT reset flag */
        rcu_all_reset_flag_clear();
        
        while(1){
        }
        
    }else{
        /* turn on LED1 */
        gd_eval_led_on(LED1);
    }

    while (1){
    }
}
