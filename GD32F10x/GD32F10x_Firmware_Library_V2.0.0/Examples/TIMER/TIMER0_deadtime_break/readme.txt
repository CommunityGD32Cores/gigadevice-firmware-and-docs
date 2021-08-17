/*!
    \file  readme.txt
    \brief description of the TIMER0 deadtime break demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

    This demo is based on the GD32107C-EVAL board, it shows how to
configure the TIMER0 peripheral to generate complementary signals, 
to insert a defined dead time value, to use the break feature and 
to lock the desired parameters.

  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 107 ,so
the TIMER0 counter clock used is 1MHz.

  The duty cycle is computed as the following description:
the channel 0 duty cycle is set to 25% so channel 0N is set to 75%.

  Insert a dead time equal to ((32 + 31) * 16 * 4) / systemcoreclock = 37.3us

  Configure the break feature, active at Low level(with GND), and using the 
automatic output enable feature.

  Use the Locking parameters level1.

  Connect the TIMER0 pins to an oscilloscope to monitor the different waveforms:
    - TIMER0_CH0  pin (PA8)
    - TIMER0_CH0N pin (PB13)

  Connect one key's pin (PB14 for example) and TIMER0_BRKIN pin PB12 with Dupont 
line. Press the key (User Key for example), the break of the wave can be observed 
through the oscilloscope.
