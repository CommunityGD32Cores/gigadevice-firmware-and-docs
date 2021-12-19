/*!
    \file    readme.txt
    \brief   description of the DBG demo
    
    \version 2021-10-30, V1.0.0, firmware for GD32W51x
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

  This demo is based on the GD32W515P-EVAL-V1.0 board, it shows that, when the 
DBG_CTL0_TM1_HOLD bit in DBG control register(DBG_CTL0) is set and the core halted,
the TIMER0 counter stop counting and the PWM outputs of all channels are stopped as
well.It's benefit for debugging.

  The TIMER0 counter clock used is 2MHz.
The Three Duty cycles are computed as the following description: 
The channel 1 duty cycle is set to 25%
The channel 2 duty cycle is set to 50%
The channel 3 duty cycle is set to 75%

  Connect the TIMER0 pins to an oscilloscope and monitor the different waveforms:
- TIMER0_CH1  pin (PA9)
- TIMER0_CH2  pin (PA10)
- TIMER0_CH3  pin (PA11)

  For Keil,in debug mode,when the core is stopped,update the register window for TIMER0,
you will see that the count value will not change. And at the same time,the PWM outputs of 
all the channels of TIMER0 will be stopped. 

