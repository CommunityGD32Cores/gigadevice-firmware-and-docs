/*!
    \file  readme.txt
    \brief description of the TIMER1 OC active demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to configure the 
TIMER peripheral to generate three different signals with three different delays.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 10800
so the TIMER1 counter clock is 10KHz.

  And generate 3 signals with 3 different delays:
  TIMER1_CH0 delay = 4000/10K  = 0.4s
  TIMER1_CH1 delay = 8000/10K  = 0.8s
  TIMER1_CH2 delay = 12000/10K = 1.2s 

  Connect the TIMER1 pins to leds and reset the board to watch the signal:
  - TIMER1_CH0  pin (PA0)
  - TIMER1_CH1  pin (PA1)
  - TIMER1_CH2  pin (PA2)
  - LED2  pin (PC2)
  - LED3  pin (PE0)
  - LED4  pin (PE1)
