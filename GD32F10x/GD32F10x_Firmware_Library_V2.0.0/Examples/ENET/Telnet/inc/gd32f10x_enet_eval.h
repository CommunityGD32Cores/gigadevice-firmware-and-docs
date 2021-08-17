/*!
    \file  gd32f10x_enet_eval.h
    \brief the header file of gd32f20x_enet_eval 
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

#ifndef GD32F10x_ENET_EVAL_H
#define GD32F10x_ENET_EVAL_H

#include "lwip/netif.h"

/* function declarations */
/* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */
void  enet_system_setup(void);

#endif /* GD32F10x_ENET_EVAL_H */
