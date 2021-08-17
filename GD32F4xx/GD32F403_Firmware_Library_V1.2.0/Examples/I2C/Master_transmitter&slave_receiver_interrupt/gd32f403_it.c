/*!
    \file  gd32f403_it.c
    \brief the header file of the ISR
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.2, firmware for GD32F403
*/

#include "gd32f403_it.h"
#include "I2C_IE.h"

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
    \brief      this function handles I2C0 event interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_EV_IRQHandler(void)
{
    I2C0_EventIRQ_Handler();
}

/*!
    \brief      this function handles I2C0 error interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_ER_IRQHandler(void)
{
    I2C0_ErrorIRQ_Handler();
}

/*!
    \brief      this function handles I2C1 event interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C1_EV_IRQHandler(void)
{
    I2C1_EventIRQ_Handler();
}

/*!
    \brief      this function handles I2C1 error interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C1_ER_IRQHandler(void)
{
    I2C1_ErrorIRQ_Handler();
}
