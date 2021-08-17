/*!
    \file  readme.txt
    \brief description of the TIMER0 complementary signals demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to configure the TIMER0 peripheral to generate three 
complementary TIMER0 signals.TIMER0CLK is fixed to systemcoreclock, the 
TIMER0 prescaler is equal to 8400 so the TIMER0 counter clock used is 20KHz.

  The Three duty cycles are computed as the following description: 
  The channel 0 duty cycle is set to 25% so channel 0N is set to 75%.
  The channel 1 duty cycle is set to 50% so channel 1N is set to 50%.
  The channel 2 duty cycle is set to 75% so channel 2N is set to 25%.   
 
  Connect the TIMER0 pins to an oscilloscope to monitor the different waveforms:
  - TIMER0_CH0  pin (PA8)
  - TIMER0_CH0N pin (PB13)  
  - TIMER0_CH1  pin (PA9)  
  - TIMER0_CH1N pin (PB14)
  - TIMER0_CH2  pin (PA10)  
  - TIMER0_CH2N pin (PB15)
