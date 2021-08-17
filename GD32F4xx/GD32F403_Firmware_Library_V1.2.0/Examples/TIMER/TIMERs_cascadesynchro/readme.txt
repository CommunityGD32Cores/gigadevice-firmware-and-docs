/*!
    \file  readme.txt
    \brief description of the TIMERs cascade synchro demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to synchronize TIMER peripherals in cascade mode.
  
  In this example three timers are used:
  1/TIMER3 is configured as master timer:
  - PWM mode is used
  - The TIMER3 update event is used as trigger output  

  2/TIMER2 is slave for TIMER3 and master for TIMER0,
  - PWM mode is used
  - The ITI3(TIMER3) is used as input trigger
  - External clock mode is used,the counter counts on the rising edges of
  the selected trigger.
  - The TIMER2 update event is used as trigger output.

  3/TIMER0 is slave for TIMER2,
  - PWM mode is used
  - The ITI1(TIMER2) is used as input trigger
  - External clock mode is used,the counter counts on the rising edges of
  the selected trigger.
 
  The TIMERxCLK is fixed to 168 MHz, the TIMER1 counter clock is :
168MHz/8400= 20 KHz.

  The master timer TIMER3 is running at TIMER3 frequency :
  TIMER3 frequency = (TIMER3 counter clock)/ (TIMER3 period + 1) = 5 Hz 
and the duty cycle = TIMER3_CH0CC/(TIMER3_CAR + 1) = 50%

  The TIMER2 is running:
  - At (TIMER1 frequency)/ (TIMER2 period + 1) = 2.5 Hz and a duty cycle
  equal to TIMER2_CH0CC/(TIMER2_CAR + 1) = 50%

  The TIMER0 is running:
  - At (TIMER2 frequency)/ (TIMER0 period + 1) = 1.25 Hz and a duty cycle
  equal to TIMER0_CH0CC/(TIMER0_CAR + 1) = 50%
  