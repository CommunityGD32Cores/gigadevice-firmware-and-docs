/*!
    \file  gd32f3x0_it.c
    \brief interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0_it.h"
#include "rc5_encode.h"
#include "rc5_decode.h"
#include "ir_decode.h"
#include "systick.h"

uint32_t icvalue1 = 0;
uint32_t icvalue2 = 0;

extern uint32_t rc5_frame_manchester_format;
extern __IO trc5_packet_struct rc5_tmp_packet;

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
    delay_decrement( );
}

/*!
    \brief      this function handles TIMER15 update interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER15_IRQHandler(void)
{
    rc5_encode_signal_generate(rc5_frame_manchester_format);

    /* clear TIMER15 update interrupt */
    timer_interrupt_flag_clear(TIMER15, TIMER_INT_UP);
}

/*!
    \brief      this function handles TIMER2 overflow and update interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    /* ic2 interrupt*/
    if((RESET != timer_interrupt_flag_get(IR_TIMER, TIMER_INT_CH1))){
        /* clear IR_TIMER CH1 interrupt */
        timer_interrupt_flag_clear(IR_TIMER, TIMER_INT_CH1);
        /* get the input capture value */
        icvalue2 = TIMER_CH1CV(IR_TIMER);
        /* rc5 */
        rc5_data_sampling(icvalue2, 1);
    }

    /* ic1 interrupt */
    if((RESET != timer_interrupt_flag_get(IR_TIMER, TIMER_INT_CH0))){
        /* clear IR_TIMER CH0 interrupt */
        timer_interrupt_flag_clear(IR_TIMER, TIMER_INT_CH0);
        /* get the input capture value */
        icvalue1 = TIMER_CH0CV(IR_TIMER) - TIMER_CH1CV(IR_TIMER);
        /* rc5 */
        rc5_data_sampling(icvalue1, 0);
    }
}
