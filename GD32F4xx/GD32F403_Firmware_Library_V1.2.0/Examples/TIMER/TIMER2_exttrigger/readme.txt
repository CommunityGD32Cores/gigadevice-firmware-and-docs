/*!
    \file  readme.txt
    \brief description of the TIMER2 external trigger demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to start TIMER peripherals with an external trigger.
  TIMER2 is configured as slave timer for an external trigger connected to 
TIMER2 CI0 pin :
  - The TIMER2 CI0FE0 is used as trigger input
  - Rising edge is used to start the TIMER2: event mode.
  - TIMER2 is used PWM0 Mode 
  The starts of the TIMER2 counter are controlled by the external trigger. 

  Connect the TIMER2 CH0 pin to +3V3 on the board as a external trigger with a
rising edge.
