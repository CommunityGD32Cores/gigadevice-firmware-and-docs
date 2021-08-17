/*!
    \file    readme.txt
    \brief   description of the TIMER1 OC active demo

    \version 2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    \version 2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    \version 2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2019-11-20, V3.2.0, firmware update for GD32F1x0(x=3,5,7,9)
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

  This demo is based on the GD32150R-EVAL-V1.3/GD32190R-EVAL-V1.2 board, it shows how to
configure the TIMER peripheral to generate three different signals with three 
different delays.
  
  The TIMxCLK frequency is set to systemcoreclock (72MHz), the prescaler is 36000
so the TIMER1 counter clock is 2KHz.

  And generate 3 signals with 3 different delays:
  TIMER1_CH0 delay = 4000/2000  = 2s
  TIMER1_CH1 delay = 8000/2000  = 4s
  TIMER1_CH2 delay = 12000/2000 = 6s 

  Connect the TIMER1 pins to leds to watch the signal:
  - TIMER1_CH0  pin (PA0)
  - TIMER1_CH1  pin (PA1)
  - TIMER1_CH2  pin (PA2)

  if use GD32150R-EVAL board:
  - LED2  pin (PC11)
  - LED3  pin (PC12)
  - LED4  pin (PD2)

  if use GD32190R-EVAL board:
  - LED2  pin (PA12)
  - LED3  pin (PB6)
  - LED4  pin (PB7)
