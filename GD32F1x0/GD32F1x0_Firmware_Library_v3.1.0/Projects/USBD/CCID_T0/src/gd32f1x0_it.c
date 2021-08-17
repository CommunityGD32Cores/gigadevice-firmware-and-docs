/*!
    \file  gd32f1x0_it.c
    \brief main interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

#include "gd32f1x0_it.h"
#include "usbd_int.h"
#include "platform_config.h"
#include "smartcard.h"
#include "systick.h"

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
    delay_decrement();
}

/*!
    \brief      this function handles USBD interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  USBD_LP_IRQHandler (void)
{
    usbd_isr();
}

/**
  * @brief  This function handles SC_USART global interrupt request.
  * @param  None
  * @retval None
  */
void SC_USART_IRQHandler(void)
{
    /* If a Frame error is signaled by the card */
    if(usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_ERR_FERR) != RESET)
    {
        usart_data_receive(SC_USART);

        /* Clear the Frame error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_ERR_FERR);

        /* Resend the byte that failed to be received (by the Smartcard) correctly */
        SC_ParityErrorHandler();
    }

    /* If the SC_USART detects a parity error */
    if(usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_PERR) != RESET)
    {
        /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
        usart_interrupt_enable(SC_USART, USART_INT_RBNE);

        /* Clear the Parity error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_PERR);

        /* Flush the SC_USART DR register */
        usart_data_receive(SC_USART);
    }

    if(usart_interrupt_flag_get(SC_USART, USART_INT_RBNE) != RESET)
    {
        /* Disable SC_USART RXNE Interrupt */
        usart_interrupt_disable(SC_USART, USART_INT_RBNE);

        usart_data_receive(SC_USART);
    }

    /* If a Overrun error is signaled by the card */
    if(usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_ERR_ORERR) != RESET)
    {
        usart_data_receive(SC_USART);

        /* Clear the Overrun error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_ERR_ORERR);
    }

    /* If a Noise error is signaled by the card */
    if(usart_interrupt_flag_get(SC_USART, USART_INT_FLAG_ERR_NERR) != RESET)
    {
        usart_data_receive(SC_USART);

        /* Clear the Noise error interrupt */
        usart_interrupt_flag_clear(SC_USART, USART_INT_FLAG_ERR_NERR);
    }
}
/**
  * @brief  This function handles the smartcard detection interrupt request.
  * @param  None
  * @retval None
  */
void SC_OFF_EXTI_IRQHandler(void)
{
    if(exti_interrupt_flag_get(SC_OFF_EXTI_LINE) != RESET)
    {
        /* Clear EXTI Line Pending Bit */
        exti_interrupt_flag_clear(SC_OFF_EXTI_LINE);

        /* Smartcard detected */
        CCID_UpdSlotChange(1);

        /* Enable CMDVCC */
        SC_PowerCmd(ENABLE);
    }
}


#ifdef USB_DEVICE_LOW_PWR_MODE_SUPPORT

/*!
    \brief      this function handles USB WakeUp interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void  USBDWakeUp_IRQHandler (void)
{
    exti_interrupt_flag_clear(EXTI_18);
}
#endif /* USB_DEVICE_LOW_PWR_MODE_SUPPORT */
