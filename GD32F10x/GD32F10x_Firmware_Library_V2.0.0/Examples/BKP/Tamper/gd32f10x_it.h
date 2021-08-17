/*!
    \file  gd32f10x_it.h
    \brief the header file of the ISR
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef GD32F10X_IT_H
#define GD32F10X_IT_H

#include "gd32f10x.h"

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
/* TAMPER handle function */
void TAMPER_IRQHandler(void);

#endif /* GD32F10X_IT_H */
