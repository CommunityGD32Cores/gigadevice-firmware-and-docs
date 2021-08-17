/*!
    \file  readme.txt
    \brief description of the standby mode wakeup through wakeup pin demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This shows pmu how to enter standby mode and wakeup it. Press tamper key enter standby 
mode, led turn off. When you press wakeup key, mcu will be wakeuped from standby mode, 
led will be turn on. when exit from the standby mode, a power-on reset occurs and the 
mcu will execute instruction code from the 0x00000000 address.
 