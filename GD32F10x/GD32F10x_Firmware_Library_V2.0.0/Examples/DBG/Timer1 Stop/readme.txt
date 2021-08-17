/*!
    \file  readme.txt
    \brief description of the DBG demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32F107C-EVAL board, it shows that, when the 
TIMER1_HOLD bit in DBG control register(DBG_CTL0) is set and the core halted,
the TIMER1 counter stop counting and the PWM outputs of all channels are stopped as
well.It's benefit for debuging.

  The TIMER1 counter clock used is 1MHz.
The Three Duty cycles are computed as the following description: 
The channel 1 duty cycle is set to 25%
The channel 2 duty cycle is set to 50%
The channel 3 duty cycle is set to 75%

  Connect the TIMER1 pins to an oscilloscope and monitor the different waveforms:
- TIMER1_CH1  pin (PB3)
- TIMER1_CH2  pin (PB10)
- TIMER1_CH3  pin (PB11)

  For Keil,in debug mode,when the core is stopped,update the register window for TIMER1,
you will see that the count value will not change. And at the same time,the PWM outputs of 
all the channels of TIMER1 will be stopped. 

