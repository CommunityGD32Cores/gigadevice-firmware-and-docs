/*!
    \file  readme.txt
    \brief description of the TIMER1 single pulse demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how 
to configure the TIMER1 peripheral in single pulse mode.
  The external signal is connected to TIMER1 CH0 pin (PA0) and the falling 
edge is used as active edge.

  The single pulse signal is output on TIMER1 CH1 pin (PA1).

  The TIMER1CLK frequency is set to systemcoreclock (84MHz for GD32F330,108MHz 
for GD32F350),the prescaler is 7(GD32F330) or 9(GD32F350),so the TIMER1 counter
clock is 12MHz.

  single pulse value = (TIMER1_Period - TIMER1_Pulse) / TIMER1 counter clock
                     = (41999 - 20999) / 12MHz = 1.75 ms.

  Connect the TIMER1 CH0 pin (PA0)to KEY,When KEY is pressed,a single pulse is generated.