/*!
    \file  readme.txt
    \brief description of the low voltage detector demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This shows how to use low voltage detector. If VDD/VDDA is higher or lower than the LVD 
threshold, then a lvd event will occur. The event is internally connected to the EXTI line 
16 and can generate an interrupt if enabled through the EXTI registers. Use external power 
connect 3.3v pin and GND pin of the board , adjust voltage higher or lower than the LVD 
hreshold 3.1v, LED1 will toggle.
 