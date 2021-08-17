/*!
    \file  readme.txt
    \brief description of MCUDGB demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows that, when the corresponding
bit(MCUDBG_TIMER1_STOP) in MCUDBG control register(DBG_CTL0) is set, When the core 
halted, the timer1 counters stopped counting and the PWM outputs of all channels are 
stopped as well.It's benefit for debuging.
  The TIMER1 counter clock used is 20KHz.
  The Three Duty cycles are computed as the following description: 
  The channel 1 duty cycle is set to 25% so channel 1N is set to 75%.
  The channel 2 duty cycle is set to 50% so channel 2N is set to 50%.
  The channel 3 duty cycle is set to 75% so channel 3N is set to 25%.

  Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
  - TIMER1_CH1  pin (PA8)
  - TIMER1_CH1N pin (PB13)
  - TIMER1_CH2  pin (PA9)
  - TIMER1_CH1N pin (PB14)
  - TIMER1_CH3  pin (PA10)
  - TIMER1_CH3N pin (PB15)

  For Keil,in debug mode,when stopped,update the register window for TIMER1,you will 
see that the count value will not change.And at the same time,the PWM outputs of all 
the channels of TIMER1,including complementary channels will be stopped.

