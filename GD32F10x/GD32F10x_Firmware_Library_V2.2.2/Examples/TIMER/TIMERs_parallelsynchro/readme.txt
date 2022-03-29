/*!
    \file    readme.txt
    \brief   description of the TIMERs parallel synchro demo for gd32e10x
    
    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

  This demo is based on the GD32107C-EVAL board, it shows how to synchronize 
TIMER peripherals in parallel mode.
  
  In this example three timers are used:
  1/TIMER1 is configured as master timer:
  - PWM mode is used.
  - The TIMER1 update event is used as trigger output.

  2/TIMER2 is slave for TIMER1, 
  - PWM mode is used.
  - The ITR1(TIMER1) is used as input trigger.
  - external clock mode is used, the counter counts on the rising edges of
  the selected trigger.

  3/TIMER0 is slave for TIMER1, 
  - PWM mode is used.
  - The ITR1(TIMER1) is used as input trigger.
  - external clock mode is used, the counter counts on the rising edges of
  the selected trigger.
 
  The TIMERxCLK is fixed to 108 MHz, the TIMER1 counter clock is:
108MHz/5400= 20KHz.

  The master timer TIMER1 CH0(PA0) is running at TIMER1 frequency:
  TIMER1 frequency = (TIMER1 counter clock)/ (TIMER1 period + 1) = 5 Hz 
and the duty cycle = TIMER1_CH0CV/(TIMER1_CAR + 1) = 50%

  The TIMER2 CH0(PA6) is running:
  - At (TIMER1 frequency)/ (TIMER2 period + 1) = 2.5 Hz and a duty cycle
  equal to TIMER2_CH0CV/(TIMER2_CAR + 1) = 50%

  The TIMER0 CH0(PA8) is running:
  - At (TIMER1 frequency)/ (TIMER0 period + 1) = 2.5 Hz and a duty cycle
  equal to TIMER2_CH0CV/(TIMER2_CAR + 1) = 50%
  