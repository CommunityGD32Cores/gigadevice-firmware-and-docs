/*!
    \file  readme.txt
    \brief description of the TIMER1 external trigger demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to start TIMER 
peripherals with an external trigger.
  TIMER1 is configured as slave timer for an external trigger connected to 
TIMER1 CI0 pin :
  - The TIMER1 CI0FE0 is used as trigger input
  - Rising edge is used to start the TIMER1: event mode.
  - TIMER1 is used PWM0 Mode 
  The starts of the TIMER1 counter are controlled by the external trigger. 

  Connect the TIMER1 CH0 pin to +3V3 on the board as a external trigger with a
rising edge.
