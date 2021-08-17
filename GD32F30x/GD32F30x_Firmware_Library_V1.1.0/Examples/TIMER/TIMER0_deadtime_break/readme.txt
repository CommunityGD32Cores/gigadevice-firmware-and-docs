/*!
    \file  readme.txt
    \brief description of the TIMER0 deadtime break demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

    This demo is based on the GD32F30x-EVAL board, it shows how to
configure the TIMER0 peripheral to generate complementary signals, 
to insert a defined dead time value, to use the break feature and 
to lock the desired parameters.

  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 119 ,so
the TIMER0 counter clock used is 1MHz.

  The duty cycle is computed as the following description:
the channel 0 duty cycle is set to 25% so channel 0N is set to 75%.

  Insert a dead time equal to (63*16*4)/systemcoreclock = 33.6us

  Configure the break feature, active at Low level(with GND), and using the 
automatic output enable feature.

  Use the Locking parameters level1.

