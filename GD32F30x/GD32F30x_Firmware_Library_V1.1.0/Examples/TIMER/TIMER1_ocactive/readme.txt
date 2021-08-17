/*!
    \file  readme.txt
    \brief description of the TIMER1 OC active demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to configure the 
TIMER peripheral to generate three different signals with three different delays.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 12000
so the TIMER1 counter clock is 10KHz.

  And generate 3 signals with 3 different delays:
  TIMER1_CH0 delay = 4000/10000  = 0.4s
  TIMER1_CH1 delay = 8000/10000  = 0.8s
  TIMER1_CH2 delay = 12000/10000 = 1.2s 

  Connect the TIMER1 pins to leds to watch the signal:
  - TIMER1_CH0  pin (PA0)
  - TIMER1_CH1  pin (PA1)
  - TIMER1_CH2  pin (PA2)
  - LED2  pin (PC7)
  - LED3  pin (PC8)
  - LED4  pin (PC9)
