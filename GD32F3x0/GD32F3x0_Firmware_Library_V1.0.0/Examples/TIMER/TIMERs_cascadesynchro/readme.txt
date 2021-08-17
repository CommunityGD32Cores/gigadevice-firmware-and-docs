/*!
    \file  readme.txt
    \brief description of the TIMERs cascade synchro demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to
synchronize TIMER peripherals in cascade mode.
  
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
 
  The TIMxCLK frequency is set to systemcoreclock 84MHz(GD32F330) or 108MHz(GD32F350),
the prescaler is 42000(GD32F330) or 54000(GD32F350) so the TIMER2 counter clock is 2KHz.

  The master timer TIMER1 is running at TIMER1 frequency :
  TIMER1 frequency = (TIMER1 counter clock)/ (TIMER1 period + 1) = 0.5 Hz 
and the duty cycle = TIMER1_CH0CC/(TIMER1_CAR + 1) = 50%

  The TIMER2 is running:
  - At (TIMER1 frequency)/ (TIMER2 period + 1) = 0.25 Hz and a duty cycle
  equal to TIMER2_CH0CC/(TIMER2_CAR + 1) = 50%

  The TIMER0 is running:
  - At (TIMER2 frequency)/ (TIMER0 period + 1) = 0.125 Hz and a duty cycle
  equal to TIMER0_CH0CC/(TIMER0_CAR + 1) = 50%
  