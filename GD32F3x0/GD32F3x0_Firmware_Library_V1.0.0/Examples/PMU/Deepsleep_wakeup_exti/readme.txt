/*!
    \file  readme.txt
    \brief description of the deepsleep wakeup through exti demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows pmu how to enter deepsleep 
mode and wakeup it. Press wakeup key to enter deepsleep mode, led stop flashing. When 
you press tamper key to generate an exti interrupt, mcu will be wakeuped from deepsleep 
mode, led sparks again. But the led sparks slower, because at this time IRC16M is the system 
clock.
