/*!
    \file  gd32f30x_it.c
    \brief main interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "gd32f30x_it.h"
#include "usbH_int.h"
#include "usb_delay.h"

extern usb_core_handle_struct usb_core_dev;
//extern uint32_t usbfs_prescaler;

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
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
}

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  USBFS_IRQHandler (void)
{
    usbh_isr (&usb_core_dev);
}

/*!
    \brief      this function handles Timer0 updata interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER0_UP_TIMER9_IRQHandler(void)
{
    timer_delay_irq();
}


