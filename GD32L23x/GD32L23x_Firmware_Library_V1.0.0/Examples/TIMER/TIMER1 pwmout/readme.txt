/*!
    \file    readme.txt
    \brief   description of the TIMER1 PWM output demo

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
configure the TIMER peripheral in PWM (Pulse Width Modulation) mode.
  
  The TIMxCLK frequency is set to CK_AHB (64MHz), the prescaler is 
63 so the TIMER1 counter clock is 1MHz.

    TIMER1 Channel0 duty cycle = (2000/ 10000)* 100 = 20%.  
    TIMER1 Channel1 duty cycle = (3000/ 10000)* 100 = 30%.
    TIMER1 Channel2 duty cycle = (5000/ 10000)* 100 = 50%.
    TIMER1 Channel3 duty cycle = (8000/ 10000)* 100 = 80%.

    Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
    - TIMER1_CH0  pin (PA0)
    - TIMER1_CH1  pin (PA1)
    - TIMER1_CH2  pin (PB10)
    - TIMER1_CH3  pin (PB11)
 