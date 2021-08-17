/*!
    \file  main.c
    \brief USART transmit and receive interrupt
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#include "gd32f403.h"
#include <stdio.h>
#include "gd32f403_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE            (ARRAYNUM(txbuffer) - 1)

uint8_t txbuffer[] = "\n\rUSART interrupt test\n\r";
uint8_t rxbuffer[32];
uint8_t tx_size = TRANSMIT_SIZE;
uint8_t rx_size = 32;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0, 0);
    /* configure COM1 */
    gd_eval_com_init(EVAL_COM1);
    /* enable USART TBE interrupt */  
    usart_interrupt_enable(USART0, USART_INT_TBE);
    
    /* wait until USART send the transmitter_buffer */
    while(txcount < tx_size);
    
    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
    
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    
    /* wait until USART receive the receiver_buffer */
    while(rxcount < rx_size);
    if(rxcount == rx_size)
        printf("\n\rUSART receive successfully!\n\r");
    while (1);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));
    return ch;
}
