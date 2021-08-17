/*!
    \file  readme.txt
    \brief description of the comparator window demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to make an analog 
  watchdog by using comparator 
  peripherals in window mode:
  - The upper threshold is set to VREFINT = 1.22V
  - The lower threshold is set to VREFINT / 2 = 1.22V = 0.610V
  - The input voltage is configured to be connected to PC1

  If the input voltage is above the higher threshold, LED1 and LED3 are turned on.
  If the input voltage is under the lower threshold, LED2 and LED4 are turned on.
  If the input voltage is within the thresholds, the MCU remains in deepsleep mode
  and all leds are turned off.

  Connect PA1 to PC1.   
