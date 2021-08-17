/*!
    \file    readme.txt
    \brief   description of HPTIMER0  DMA mode example

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

  This example is based on the GD32E507Z-EVAL-V1.0 board. This example shows how to
use DMA with Slave_TIMER0 counter reset request to transfer data from memory to 
Slave_TIMER0 compare register 0~1.

  The ST0_CH0_O channel (PA8) and ST0_CH1_O channel (PA9) output a pair of complementar 
PWM waveforms with a period of 1us. The waveforms on pins of PA8 and PA9 can be observed
by oscilloscope.

  Slave_TIMER0 compare register 0~1 are to be updated once per circle.On the first reset 
DMA request, buffer[0] is transferred to HPTIMER_ST0CMP0V and buffer[1] is transferred 
to HPTIMER_ST0CMP1V. The  high level width of ST0_CH0_O channel (PA8) is 434ns.On the 
second reset DMA request,buffer[2] is transferred to HPTIMER_ST0CMP0V and buffer[3] is
transferred to HPTIMER_ST0CMP1V. The  high level width of ST0_CH0_O channel (PA8) is 217ns.
