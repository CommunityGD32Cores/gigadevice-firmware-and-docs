/*!
    \file  gd32f20x_enet_eval.h
    \brief the header file of gd32f20x_enet_eval 
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

#ifndef GD32F20x_ENET_EVAL_H
#define GD32F20x_ENET_EVAL_H

#include "lwip/netif.h"

/* function declarations */
/* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */
void  enet_system_setup(void);

#endif /* GD32F20x_ENET_EVAL_H */
