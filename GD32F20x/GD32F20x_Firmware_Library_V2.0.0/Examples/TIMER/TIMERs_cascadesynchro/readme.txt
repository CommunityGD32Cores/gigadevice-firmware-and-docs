/*!
    \file  readme.txt
    \brief description of the TIMERs cascade synchro demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to synchronize 
TIMER peripherals in cascade mode.
  
  In this example three timers are used:
  1/TIMER1 is configured as master timer:
  - PWM mode is used
  - The TIMER1 update event is used as trigger output  

  2/TIMER2 is slave for TIMER1 and master for TIMER0,
  - PWM mode is used
  - The ITR0(TIMER1) is used as input trigger 
  - External clock mode is used,the counter counts on the rising edges of
  the selected trigger.
  - The TIMER2 update event is used as trigger output.

  3/TIMER0 is slave for TIMER2,
  - PWM mode is used
  - The ITR1(TIMER2) is used as input trigger
  - External clock mode is used,the counter counts on the rising edges of
  the selected trigger.
 
  The TIMERxCLK is fixed to 120 MHz, the TIMER1 counter clock is :
120MHz/6000= 20 KHz.

  The master timer TIMER1 is running at TIMER1 frequency :
  TIMER1 frequency = (TIMER1 counter clock)/ (TIMER1 period + 1) = 5 Hz 
and the duty cycle = TIMER1_CH0CC/(TIMER1_CAR + 1) = 50%

  The TIMER2 is running:
  - At (TIMER1 frequency)/ (TIMER2 period + 1) = 2.5 Hz and a duty cycle
  equal to TIMER2_CH0CC/(TIMER2_CAR + 1) = 50%

  The TIMER0 is running:
  - At (TIMER2 frequency)/ (TIMER0 period + 1) = 1.25 Hz and a duty cycle
  equal to TIMER0_CH0CC/(TIMER0_CAR + 1) = 50%
  
  Connect the three pins to a logic analyzer to monitor the different waveforms:
  - TIMER1_CH1  pin (PA1)
  - TIMER2_CH0  pin (PA6)
  - TIMER0_CH0  pin (PA8)
