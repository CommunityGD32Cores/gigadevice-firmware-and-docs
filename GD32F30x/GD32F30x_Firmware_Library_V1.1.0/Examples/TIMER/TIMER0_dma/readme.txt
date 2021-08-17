/*!
    \file  readme.txt
    \brief description of the TIMER0 DMA demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to
use DMA with TIMER0 update request to transfer data from memory 
to TIMER0 capture compare register 0.
  
  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 119
so the TIMER0 counter clock used is 1MHz.

  The objective is to configure TIMER0 channel 0(PA8) to generate PWM signal with
a frequency equal to 1KHz and a variable duty cycle(25%,50%,75%) that is changed
by the DMA after a specific number of Update DMA request.

  The number of this repetitive requests is defined by the TIMER0 repetition counter,
each 2 update requests, the TIMER0 Channel 0 duty cycle changes to the next new 
value defined by the buffer.