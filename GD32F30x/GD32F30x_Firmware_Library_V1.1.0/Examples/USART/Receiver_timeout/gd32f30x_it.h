/*!
    \file  gd32f30x_it.h
    \brief the header file of the ISR
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#ifndef GD32F30X_IT_H
#define GD32F30X_IT_H

#include "gd32f30x.h"

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
/* USART0 handle function */
void USART0_IRQHandler(void);

#endif /* GD32F30X_IT_H */
