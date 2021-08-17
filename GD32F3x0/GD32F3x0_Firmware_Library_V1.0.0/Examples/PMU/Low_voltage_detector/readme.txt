/*!
    \file  readme.txt
    \brief description of the low voltage detector demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows how to use low voltage detector. 
If VDD/VDDA is higher or lower than the LVD threshold, then a lvd event will occur. The event
is internally connected to the EXTI line 16 and can generate an interrupt if enabled through
the EXTI registers. Use external power connect 3.3v pin and GND pin of the board , adjust 
voltage higher than the LVD threshold 2.9v, or lower than 2.9-0.1 = 2.8V, LED1 will toggle.
 