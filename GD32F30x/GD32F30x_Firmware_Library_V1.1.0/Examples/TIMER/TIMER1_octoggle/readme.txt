/*!
    \file  readme.txt
    \brief description of the TIMER1 OC toggle demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to configure the 
TIMER peripheral to generate the toggle signal.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 6000
so the TIMER1 counter clock is 20KHz.
  
  TIMER1 configuration: output compare toggle mode:
CH1 update rate = TIMER1 counter clock / CH1CC = 20000/4000 = 5 Hz. 
