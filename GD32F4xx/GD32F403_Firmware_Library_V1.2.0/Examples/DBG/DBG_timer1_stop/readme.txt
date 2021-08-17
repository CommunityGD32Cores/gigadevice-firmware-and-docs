/*!
    \file  readme.txt
    \brief description of the DBG demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows that, when the TIMER0_HOLD bit in DBG control register(DBG_CTL0) 
is set and the core halted,the TIMER0 counter stop counting and the PWM outputs of
all channels are stopped as well. It's benefit for debuging.

  The TIMER0 counter clock used is 1MHz.
The Three Duty cycles are computed as the following description: 
The channel 0 duty cycle is set to 25%
The channel 1 duty cycle is set to 50%
The channel 2 duty cycle is set to 75%

  Connect the TIMER0 pins to an oscilloscope and monitor the different waveforms:
- TIMER0_CH0  pin (PA8)
- TIMER0_CH1  pin (PA9)
- TIMER0_CH2  pin (PA10)

  For Keil,in debug mode,when the core is stopped,update the register window for TIMER0,
you will see that the count value will not change. And at the same time,the PWM outputs of 
all the channels of TIMER0 will be stopped. 

