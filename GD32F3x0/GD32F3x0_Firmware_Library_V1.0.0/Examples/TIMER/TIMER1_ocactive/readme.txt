/*!
    \file  readme.txt
    \brief description of the TIMER1 OC active demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to
configure the TIMER peripheral to generate three different signals with three 
different delays.
  
  The TIMxCLK frequency is set to systemcoreclock , the prescaler is 42000(GD32F330)
or 54000(GD32F350) so the TIMER1 counter clock is 2KHz.

  And generate 3 signals with 3 different delays:
  TIMER1_CH0 delay = 4000/2000  = 2s
  TIMER1_CH1 delay = 8000/2000  = 4s
  TIMER1_CH2 delay = 12000/2000 = 6s 

  Connect the TIMER1 pins to leds to watch the signal:
  - TIMER1_CH0  pin (PA0)
  - TIMER1_CH1  pin (PA1)
  - TIMER1_CH2  pin (PA2)
