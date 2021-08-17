/*!
    \file    readme.txt
    \brief   description of the TIMER2 OC inactive demo for gd32f1x0

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

  This demo is based on the GD32150R-EVAL-V1.3/GD32190R-EVAL-V1.2 board, it shows how to configure the 
TIMER peripheral to generate three different signals with three different delays.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 36000, so the TIMER1 
counter clock is 2KHz.

  And generate 3 signals with 3 different delays:
  TIMER2_CH0 delay = 2000/2000 = 1s.
  TIMER2_CH1 delay = 4000/2000 = 2s.
  TIMER2_CH2 delay = 6000/2000 = 3s.

  So PA6 is reset after a delay equal to 1s, PA7 is reset after a delay equal to 2s,
and PB0 is reset after a delay equal to 3s.

  While the counter is lower than the TIMER_CHxCV registers values, which determines
the output delay. When the counter value reaches the TIMER_CHxCV registers values, 
the channel output compare interrupts are generated,and in the handler routine, 
these LEDs are turned on as follows:
  reaches the TIMER_CH0CV - LED1 on, LED2 off
  reaches the TIMER_CH1CV - LED1 off, LED2 on
  reaches the TIMER_CH2CV - LED1 on, LED2 on
