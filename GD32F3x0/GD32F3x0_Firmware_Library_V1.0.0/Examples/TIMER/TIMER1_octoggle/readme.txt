/*!
    \file  readme.txt
    \brief description of the TIMER1 OC toggle demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to
configure the TIMER peripheral to generate the toggle signal.
  
  The TIMxCLK frequency is set to systemcoreclock , the prescaler is 42000(GD32F330)
or 54000(GD32F350) so the TIMER1 counter clock is 2KHz.
  
  TIMER1 configuration: output compare toggle mode:
  CH1 update rate = TIMER1 counter clock / CH1CC = 2000/4000 = 0.5 Hz. 
