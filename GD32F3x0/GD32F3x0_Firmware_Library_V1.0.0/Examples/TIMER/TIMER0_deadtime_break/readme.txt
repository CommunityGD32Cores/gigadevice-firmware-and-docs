/*!
    \file  readme.txt
    \brief description of the TIMER0 deadtime break demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

    This demo is based on the GD32350R-EVAL board, it shows how to configure the TIMER0 
peripheral to generate complementary signals, to insert a defined dead time value, to use 
the break feature and to lock the desired parameters.

  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 84(GD32F330)
or 107(GD32F350) ,so the TIMER0 counter clock used is 1MHz.

  The duty cycle is computed as the following description:
the channel 0 duty cycle is set to 25% so channel 0N is set to 75%.

  Insert a dead time equal to 48us(GD32F330) or 37.3us(GD32F350).

  Configure the break feature, active at Low level(with GND), and using the 
automatic output enable feature.

  Use the Locking parameters level1.
