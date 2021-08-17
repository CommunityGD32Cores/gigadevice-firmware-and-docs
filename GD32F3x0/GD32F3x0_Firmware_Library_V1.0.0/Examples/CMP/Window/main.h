/*!
    \file  main.c
    \brief the header file of the main
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#ifndef MAIN_H
#define MAIN_H

typedef enum
{
    STATE_OVER_THRESHOLD ,
    STATE_WITHIN_THRESHOLD, 
    STATE_UNDER_THRESHOLD 
} cmp_state_enum;

cmp_state_enum check_state(void);

#endif /* MAIN_H */
