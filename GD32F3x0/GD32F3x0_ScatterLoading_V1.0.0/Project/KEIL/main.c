/*!
    \file  main.c
    \brief scatter loading demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
#include "hw_config.h"
#include "gd32f3x0_eval.h"

void delay(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    interrupt_config();
    gd_eval_led_init(LED1);
    while(1){
        gd_eval_led_toggle(LED1);
        delay();
    }
}

/* load the function to section("delay") of the main.o  */
/*!
    \brief      delay program
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay(void)__attribute__((section("delay"))); 
void delay(void)
{
    uint32_t i;
    for(i=0;i<0x2fffff;i++);
}
