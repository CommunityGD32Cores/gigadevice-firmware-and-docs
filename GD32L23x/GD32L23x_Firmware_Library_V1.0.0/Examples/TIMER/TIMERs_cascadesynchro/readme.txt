/*!
    \file    readme.txt
    \brief   description of the TIMERs cascade synchro demo

    \version 2021-08-04, V1.0.0, firmware for GD32L23x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

  This example is based on the gd32l301K-START-V1.1 board, it shows how to
synchronize TIMER peripherals in cascade mode.
  
    /* timers synchronisation in cascade mode ----------------------------
       1/TIMER2 is configured as master timer:
       - PWM mode is used
       - The TIMER1 update event is used as trigger output  

       2/TIMER1 is slave for TIMER2 and master for TIMER8,
       - PWM mode is used
       - The ITI0(TIMER2) is used as input trigger 
       - external clock mode is used,the counter counts on the rising edges of
         the selected trigger.
       - the TIMER1 update event is used as trigger output. 

       3/TIMER8 is slave for TIMER1,
       - PWM mode is used
       - The ITI0(TIMER1) is used as input trigger
       - external clock mode is used,the counter counts on the rising edges of
         the selected trigger.
      -------------------------------------------------------------------- */
  The TIMxCLK frequency is set to CK_AHB 64MHz
the prescaler is 31 so the TIMER2 counter clock is 1MHz.

  The master timer TIMER2 is running at TIMER2 frequency :
  TIMER1 frequency = (TIMER2 counter clock)/ (TIMER2 period + 1) = 1000 Hz 
and the duty cycle = TIMER2_CH0CC/(TIMER2_CAR + 1) = 50%

  The TIMER1 is running:
  - At (TIMER1 frequency)/ (TIMER1 period + 1) = 500 Hz and a duty cycle
  equal to TIMER1_CH0CC/(TIMER1_CAR + 1) = 50%

  The TIMER8 is running:
  - At (TIMER8 frequency)/ (TIMER8 period + 1) = 250 Hz and a duty cycle
  equal to TIMER8_CH0CC/(TIMER8_CAR + 1) = 50%
  