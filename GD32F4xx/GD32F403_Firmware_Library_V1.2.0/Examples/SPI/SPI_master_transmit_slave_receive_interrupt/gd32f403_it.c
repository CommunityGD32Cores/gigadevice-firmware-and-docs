/*!
    \file  gd32f403_it.c
    \brief interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403_it.h"

#define arraysize         10
extern uint8_t spi0_send_array[ ];
extern uint8_t spi1_receive_array[ ];
extern uint32_t send_n, receive_n;

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
    \brief      this function handles SPI0 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI0_IRQHandler(void)
{
    if(RESET != spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_TBE)){
        /* send data */
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
        spi_i2s_data_transmit(SPI0, spi0_send_array[send_n++]);

        if(arraysize == send_n){
            spi_i2s_interrupt_disable(SPI0, SPI_I2S_INT_TBE);
        }
    }
}

/*!
    \brief      this function handles SPI1 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI1_IRQHandler(void)
{
    /* received data */
    if(RESET != spi_i2s_interrupt_flag_get(SPI1, SPI_I2S_INT_FLAG_RBNE)){
        spi1_receive_array[receive_n++] = spi_i2s_data_receive(SPI1);
    }
}
