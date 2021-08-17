/*!
    \file  readme.txt
    \brief description of the TIMER1 PWM output demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

    This demo is based on the GD32F30x-EVAL board, it shows how to configure the 
TIMER peripheral in PWM (Pulse Width Modulation) mode.
  
    The TIMxCLK frequency is set to systemcoreclock, the prescaler is 120 so the 
TIMER1 counter clock is 1MHz.
  
    TIMER1 Channel0 duty cycle = (4000/ 16000)* 100 = 25%.
    TIMER1 Channel1 duty cycle = (8000/ 16000)* 100 = 50%.
    TIMER1 Channel2 duty cycle = (12000/ 16000)* 100 = 75%.

  @note
  Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
    - TIMER1_CH1  pin (PA0)
    - TIMER1_CH2  pin (PA1)
    - TIMER1_CH3  pin (PA2)
