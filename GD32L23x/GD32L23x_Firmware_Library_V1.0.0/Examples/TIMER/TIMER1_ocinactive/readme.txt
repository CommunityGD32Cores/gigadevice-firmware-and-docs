/*!
    \file    readme.txt
    \brief   description of the TIMER1 OC inactive demo

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

  This example is based on the gd32l301K-START-V1.1 board, it shows how to configure the 
TIMER peripheral to generate three different signals with three different delays.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 6399, so the 
TIMER1 counter clock is 10KHz.

  And generate 3 signals with 3 different delays:
  TIMER1_CH0 delay = 4000/10000 = 0.4s.
  TIMER1_CH1 delay = 8000/10000 = 0.8s.
  TIMER1_CH2 delay = 12000/10000 = 1.2s.

  So PA0 is reset after a delay equal to 0.4s, PB3 is reset after a delay equal to 0.8s,
and PB10 is reset after a delay equal to 1.2s.

  While the counter is lower than the TIMER_CHxCV registers values, which determines
the output delay, the LED1, LED2 and LED3 are turned on. When the counter value reaches
the TIMER_CHxCV registers values, the channel output compare interrupts are generated,
and in the handler routine, these LEDs are turned off.
