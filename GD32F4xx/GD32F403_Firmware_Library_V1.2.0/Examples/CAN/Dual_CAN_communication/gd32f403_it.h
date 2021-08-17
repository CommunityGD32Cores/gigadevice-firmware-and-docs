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
/* NMI handle function */
void NMI_Handler(void);
/* HardFault handle function */
void HardFault_Handler(void);
/* MemManage handle function */
void MemManage_Handler(void);
/* BusFault handle function */
void BusFault_Handler(void);
/* UsageFault handle function */
void UsageFault_Handler(void);
/* SVC handle function */
void SVC_Handler(void);
/* DebugMon handle function */
void DebugMon_Handler(void);
/* PendSV handle function */
void PendSV_Handler(void);
/* SysTick handle function */
void SysTick_Handler(void);
/* CAN0 RX0 handle function */
void CAN0_RX0_IRQHandler(void);
/* CAN1 RX0 handle function */
void CAN1_RX0_IRQHandler(void);

#endif /* GD32F403_IT_H */
