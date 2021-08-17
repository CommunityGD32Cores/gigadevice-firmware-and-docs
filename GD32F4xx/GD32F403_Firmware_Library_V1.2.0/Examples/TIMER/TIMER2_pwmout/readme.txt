/*!
    \file  readme.txt
    \brief description of the TIMER2 PWM output demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

    This demo shows how to configure the TIMER peripheral in PWM (Pulse Width Modulation) mode.
  
    The TIMxCLK frequency is set to systemcoreclock, the prescaler is 168 so the 
TIMER1 counter clock is 1MHz.
  
    TIMER2 Channel0 duty cycle = (4000/ 16000)* 100 = 25%.
    TIMER2 Channel1 duty cycle = (8000/ 16000)* 100 = 50%.
    TIMER2 Channel2 duty cycle = (12000/ 16000)* 100 = 75%.

  @note
  Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
    - TIMER2_CH1  pin (PA6)
    - TIMER2_CH2  pin (PA7)
    - TIMER2_CH3  pin (PB0)
