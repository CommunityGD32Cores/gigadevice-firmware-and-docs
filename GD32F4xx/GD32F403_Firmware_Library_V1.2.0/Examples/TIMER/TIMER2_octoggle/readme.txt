/*!
    \file  readme.txt
    \brief description of the TIMER2 OC toggle demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to configure the TIMER peripheral to generate the toggle signal.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 8400
so the TIMER2 counter clock is 20KHz.
  
  TIMER2 configuration: output compare toggle mode:
CH1 update rate = TIMER2 counter clock / CH1CC = 20000/4000 = 5 Hz. 
