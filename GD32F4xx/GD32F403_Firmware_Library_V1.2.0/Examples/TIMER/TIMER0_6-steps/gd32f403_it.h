/*!
    \file  gd32f403_it.h
    \brief the header file of the ISR
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

#ifndef GD32F403_IT_H
#define GD32F403_IT_H

#include "gd32f403.h"

/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles HardFault exception */
void HardFault_Handler(void);
/* this function handles MemManage exception */
void MemManage_Handler(void);
/* this function handles BusFault exception */
void BusFault_Handler(void);
/* this function handles UsageFault exception */
void UsageFault_Handler(void);
/* this function handles SVC exception */
void SVC_Handler(void);
/* this function handles DebugMon exception */
void DebugMon_Handler(void);
/* this function handles PendSV exception */
void PendSV_Handler(void);
/* this function handles SysTick exception */
void SysTick_Handler(void);
/* TIMER0 handle function */
void TIMER0_TRG_CMT_TIMER10_IRQHandler(void);

#endif /* GD32F403_IT_H */
