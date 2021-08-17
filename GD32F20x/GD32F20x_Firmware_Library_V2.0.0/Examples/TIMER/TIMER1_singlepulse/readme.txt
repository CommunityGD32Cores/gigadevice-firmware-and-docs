/*!
    \file  readme.txt
    \brief description of the TIMER1 single pulse demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to configure 
the TIMER1 peripheral in single pulse mode.
  The external signal is connected to TIMER1 CH0 pin (PA0) and the falling 
edge is used as active edge.

  The single pulse signal is output on TIMER1 CH1 pin (PA1).

  The TIMER1CLK frequency is set to systemcoreclock,the prescaler is 6,so the 
TIMER1 counter clock is 20MHz.
  
  single pulse value = (TIMER1_period - TIMER1_pulse) / TIMER1 counter clock
                     = (65535 - 11535) / 20MHz = 2.7 ms.

  Connect the TIMER1 CH1 pin (PA1) to an oscilloscope to monitor the waveform.
Connect the TIMER1 CH0 pin (PA0) to KEY_TAMPER(PC13),When KEY_TAMPER is pressed,
a single pulse is generated on TIMER1 CH1 pin.