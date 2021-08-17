/*!
    \file  readme.txt
    \brief description of the TIMER1 single pulse demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to configure the TIMER2 peripheral in single pulse mode.
  The external signal is connected to TIMER2 CH0 pin (PA6) and the falling 
edge is used as active edge.

  The single pulse signal is output on TIMER2 CH1 pin (PA7).

  The TIMER2CLK frequency is set to systemcoreclock,the prescaler is 84,so the 
TIMER2 counter clock is 2MHz.
  
  single pulse value = (TIMER2_period - TIMER2_pulse) / TIMER2 counter clock
                     = (65535 - 11535) / 2MHz = 27 ms.

  Connect the TIMER2 CH0 pin (PA6)to KEY,When KEY is pressed,
a single pulse is generated.