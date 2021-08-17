/*!
    \file  readme.txt
    \brief description of the TIMER2 OC active demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to configure the TIMER peripheral to generate three 
different signals with three different delays.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 16800
so the TIMER2 counter clock is 10KHz.

  And generate 3 signals with 3 different delays:
  TIMER2_CH0 delay = 4000/10000  = 0.4s
  TIMER2_CH1 delay = 8000/10000  = 0.8s
  TIMER2_CH2 delay = 12000/10000 = 1.2s 

  Connect the TIMER2 pins to leds to watch the signal:
  - TIMER2_CH0  pin (PA6)
  - TIMER2_CH1  pin (PA7)
  - TIMER2_CH2  pin (PB0)
