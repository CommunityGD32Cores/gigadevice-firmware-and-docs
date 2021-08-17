/*!
    \file  readme.txt
    \brief description of the standby mode wakeup through wakeup pin demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32207i-EVAL board, it shows pmu how to enter standby 
mode and wakeup it. Press tamper key enter standby mode, led turn off. When you press 
wakeup key, mcu will be wakeuped from standby mode, led will be turn on. when exit from
the standby mode, a power-on reset occurs and the mcu will execute instruction code 
from the 0x00000000 address.
 