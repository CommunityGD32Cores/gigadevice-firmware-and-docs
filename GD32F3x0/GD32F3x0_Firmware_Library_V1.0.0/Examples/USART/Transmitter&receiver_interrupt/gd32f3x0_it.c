/*!
    \file  gd32f3x0_it.c
    \brief interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/


#include "gd32f3x0_it.h"
#include "gd32f3x0_eval.h"

extern uint8_t transfersize;
extern uint8_t receivesize;
extern __IO uint8_t txcount; 
extern __IO uint16_t rxcount; 
extern uint8_t receiver_buffer[32];
extern uint8_t transmitter_buffer[];
/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles USART RBNE interrupt request and TBE interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(EVAL_COM1, USART_INT_FLAG_RBNE)){
        /* receive data */
        receiver_buffer[rxcount++] = (usart_data_receive(EVAL_COM1) & 0x7F);
        if(rxcount == receivesize){
            usart_interrupt_disable(EVAL_COM1, USART_INT_RBNE);
        }
    }

    if(RESET != usart_interrupt_flag_get(EVAL_COM1, USART_INT_FLAG_TBE)){
        /* transmit data */
        usart_data_transmit(EVAL_COM1, transmitter_buffer[txcount++]);
        if(txcount == transfersize){
            usart_interrupt_disable(EVAL_COM1, USART_INT_TBE);
        }
    }
}
