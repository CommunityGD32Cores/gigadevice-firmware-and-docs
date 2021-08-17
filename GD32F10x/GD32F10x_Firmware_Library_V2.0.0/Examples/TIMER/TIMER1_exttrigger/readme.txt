/*!
    \file  readme.txt
    \brief description of the TIMER1 external trigger demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to start TIMER 
peripherals with an external trigger.
  TIMER1 is configured as slave timer for an external trigger connected to 
TIMER1 TI0 pin :
  - The TIMER1 TI0FP0 is used as trigger input
  - Rising edge is used to start the TIMER1: trigger mode.
  - TIMER1 is used PWM0 Mode 
  The starts of the TIMER1 counter are controlled by the external trigger. 

  Connect the TIMER1 CH2 pin (PA2) to an oscilloscope to monitor the waveform.
Connect the TIMER1 CH0 pin PA0 to +3V3 on the board as a external trigger with a
rising edge.
