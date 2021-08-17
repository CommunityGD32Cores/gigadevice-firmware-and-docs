/*!
    \file  readme.txt
    \brief description of the TIMER1 PWM output demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

    This demo is based on the GD32350R-EVAL board, it shows how to
configure the TIMER peripheral in PWM (Pulse Width Modulation) mode.
  
    The TIMxCLK frequency is set to systemcoreclock (84MHz), the prescaler is 
84(GD32F330) or 108(GD32F350) so the TIMER1 counter clock is 1MHz.
  
    TIMER1 Channel1 duty cycle = (4000/ 16000)* 100 = 25%.
    TIMER1 Channel2 duty cycle = (8000/ 16000)* 100 = 50%.
    TIMER1 Channel3 duty cycle = (12000/ 16000)* 100 = 75%.

    Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
    - TIMER1_CH1  pin (PB3)
    - TIMER1_CH2  pin (PB10)
    - TIMER1_CH3  pin (PB11)
