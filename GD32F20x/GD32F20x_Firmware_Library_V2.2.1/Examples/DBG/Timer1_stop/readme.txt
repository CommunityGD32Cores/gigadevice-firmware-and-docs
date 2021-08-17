/*!
    \file    readme.txt
    \brief   description of MCUDGB demo

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
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

  This demo is based on the GD32207i-EVAL-V1.2 board, it shows that, when the
corresponding bit(MCUDBG_TIMER1_STOP) in MCUDBG control register(DBG_CTL0) is set, When the core 
halted, the timer1 counters stopped counting and the PWM outputs of all channels are 
stopped as well.It's benefit for debuging.
  The TIMER1 counter clock used is 20KHz.
  The Three Duty cycles are computed as the following description: 
  The channel 1 duty cycle is set to 25% so channel 1N is set to 75%.
  The channel 2 duty cycle is set to 50% so channel 2N is set to 50%.
  The channel 3 duty cycle is set to 75% so channel 3N is set to 25%.

  Connect the TIMER1 pins to an oscilloscope to monitor the different waveforms:
  - TIMER1_CH1  pin (PA8)
  - TIMER1_CH1N pin (PB13)
  - TIMER1_CH2  pin (PA9)
  - TIMER1_CH1N pin (PB14)
  - TIMER1_CH3  pin (PA10)
  - TIMER1_CH3N pin (PB15)

  For Keil,in debug mode,when stopped,update the register window for TIMER1,you will 
see that the count value will not change.And at the same time,the PWM outputs of all 
the channels of TIMER1,including complementary channels will be stopped.

