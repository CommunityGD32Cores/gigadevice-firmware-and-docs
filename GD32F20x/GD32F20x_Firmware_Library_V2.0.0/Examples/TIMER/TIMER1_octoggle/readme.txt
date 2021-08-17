/*!
    \file  readme.txt
    \brief description of the TIMER1 OC toggle demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to configure the 
TIMER peripheral to generate the toggle signal.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 6000
so the TIMER1 counter clock is 20KHz.
  
  TIMER1 configuration: output compare toggle mode:
CH1 update rate = TIMER1 counter clock / CH1CC = 20000/4000 = 5 Hz. 

  Connect the TIMER1 CH1 pin (PA1) to an oscilloscope to monitor the waveform.