/*!
    \file  gd32f30x_enet_eval.h
    \brief the header file of gd32f30x_enet_eval 
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#ifndef GD32F30x_ENET_EVAL_H
#define GD32F30x_ENET_EVAL_H

#include "netif.h"

/* function declarations */
/* setup ethernet system(GPIOs, clocks, MAC, DMA, systick) */
void  enet_system_setup(void);

#endif /* GD32F30x_ENET_EVAL_H */
