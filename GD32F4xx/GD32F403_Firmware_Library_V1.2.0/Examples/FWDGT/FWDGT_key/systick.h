/*!
    \file  systick.h
    \brief the header file of systick
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

#ifndef SYS_TICK_H
#define SYS_TICK_H

#include <stdint.h>

/* configure systick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_1ms(uint32_t count);
/* delay decrement */
void delay_decrement(void);

#endif /* SYS_TICK_H */
