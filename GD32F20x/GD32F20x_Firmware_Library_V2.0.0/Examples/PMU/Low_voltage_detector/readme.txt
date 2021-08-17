/*!
    \file  readme.txt
    \brief description of the low voltage detector demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32207i-EVAL board, it shows how to use low voltage detector. 
If VDD/VDDA is higher or lower than the LVD threshold, then a lvd event will occur. The event
is internally connected to the EXTI line 16 and can generate an interrupt if enabled through
the EXTI registers. Use external power connect 3.3v pin and GND pin of the board , adjust 
voltage higher or lower than the LVD threshold 3.1v, LED1 will toggle.
 