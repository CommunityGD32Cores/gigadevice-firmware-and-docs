/*!
    \file  readme.txt
    \brief description of the TIMER1 PWM output demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

    This demo is based on the GD32107C-EVAL board, it shows how to configure the 
TIMER peripheral in PWM (Pulse Width Modulation) mode.
  
    The TIMxCLK frequency is set to systemcoreclock, the prescaler is 108 so the 
TIMER1 counter clock is 1MHz.
  
    TIMER1 Channel1 duty cycle = (4000/ 16000)* 100 = 25%.
    TIMER1 Channel2 duty cycle = (8000/ 16000)* 100 = 50%.
    TIMER1 Channel3 duty cycle = (12000/ 16000)* 100 = 75%.

  Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
    - TIMER1_CH1  pin (PA1)
    - TIMER1_CH2  pin (PA2)
    - TIMER1_CH3  pin (PA3)
