/*!
    \file  GD32F403_it.c
    \brief main interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403_it.h"
#include "usbd_int.h"
#include "usb_delay.h"
#include "custom_hid_core.h"

uint8_t send_buffer1[4] = {0x00, 0x01, 0x00, 0x00};
uint8_t send_buffer2[4] = {0x00, 0x01, 0x00, 0x00};

extern usb_core_handle_struct usbhs_core_dev;
extern uint32_t usbfs_prescaler;

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
    \brief      this function handles EXTI0_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  EXTI0_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(WAKEUP_KEY_EXTI_LINE)) {
        if (USB_STATUS_CONFIGURED == usbhs_core_dev.dev.status) {
            send_buffer1[0] = 0x15; 

            if (RESET == gd_eval_key_state_get(KEY_WAKEUP)) {
                if(send_buffer1[1]) {
                    send_buffer1[1] = 0x00;
                } else {
                    send_buffer1[1] = 0x01;
                }
            }

            custom_hid_report_send (&usbhs_core_dev, send_buffer1, 2);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(WAKEUP_KEY_EXTI_LINE);
    }
}


/*!
    \brief      this function handles EXTI10_15_IRQ Handler.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  EXTI10_15_IRQHandler (void)
{
    if (RESET != exti_interrupt_flag_get(TAMPER_KEY_EXTI_LINE)) {
        if (USB_STATUS_CONFIGURED == usbhs_core_dev.dev.status) {
            send_buffer2[0] = 0x16;

            if (RESET == gd_eval_key_state_get(KEY_TAMPER)) {
                if(send_buffer2[1]) {
                    send_buffer2[1] = 0x00;
                } else {
                    send_buffer2[1] = 0x01;
                }
            }

            custom_hid_report_send (&usbhs_core_dev, send_buffer2, 2);
        }

        /* clear the EXTI line interrupt flag */
        exti_interrupt_flag_clear(TAMPER_KEY_EXTI_LINE);
    }
}

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  USBFS_IRQHandler (void)
{
    usbd_isr (&usbhs_core_dev);
}

/*!
    \brief      this function handles USBD wakeup interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (usbhs_core_dev.cfg.low_power) {
        SystemInit();
        rcu_usb_clock_config(usbfs_prescaler);

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_ungate(&usbhs_core_dev);
    }

    exti_interrupt_flag_clear(EXTI_18);
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


