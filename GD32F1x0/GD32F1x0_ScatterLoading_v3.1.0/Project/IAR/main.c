/*!
    \file  main.c
    \brief scatter loading demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/
  
#include "gd32f1x0.h"
#include "hw_config.h"
#include "gd32f1x0_eval.h"
#include <stdio.h>

extern const char constdata[];

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
    gd_eval_com_init(EVAL_COM1);
    
    printf("constdata address is 0x%x\n\r",(uint32_t)constdata);
    printf("delay address is 0x%x\n\r",(uint32_t)delay);
    
    while(1){
      gd_eval_led_toggle(LED1);
      delay();
    }
}

/* load the function to section("funflash") */ 
/*!
    \brief      delay program
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay(void) @".funflash"
{
    uint32_t i;
    for(i=0;i<0x2fffff;i++);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM1,USART_FLAG_TC));
    return ch;
}


