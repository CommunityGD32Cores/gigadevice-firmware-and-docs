/*!
    \file  gd32f403_it.c
    \brief interrupt service routines
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#include "gd32f403_it.h"
#include "gd32f403_eval.h"

extern uint32_t is_backup_register_clear(void);

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
    \brief      this function handles Tamper interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TAMPER_IRQHandler(void)
{
    if(RESET != bkp_interrupt_flag_get(BKP_INT_FLAG_TAMPER))
    {    
        /* a tamper detection event occurred */
        /* check if backup data registers are cleared */
        if(0 == is_backup_register_clear()){
            /* backup data registers are cleared */
            /* turn on LED3 */
            gd_eval_led_on(LED3);
        }else{
            /* backup data registers are not cleared */
            /* turn on LED4 */
            gd_eval_led_on(LED4);
        }
        /* clear the interrupt bit flag of tamper interrupt */
        bkp_interrupt_flag_clear(BKP_INT_FLAG_TAMPER);
        /* clear the bit flag of tamper event */
        bkp_flag_clear(BKP_FLAG_TAMPER);
        /* disable the tamper pin */
        bkp_tamper_interrupt_disable();
        /* enable the tamper pin */
        bkp_tamper_interrupt_enable();
        /* tamper pin active level set */
        bkp_tamper_active_level_set(TAMPER_PIN_ACTIVE_LOW);
    }
}
