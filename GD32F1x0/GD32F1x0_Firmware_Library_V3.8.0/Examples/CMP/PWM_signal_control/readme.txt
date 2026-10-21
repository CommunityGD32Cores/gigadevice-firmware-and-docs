/*!
    \file    readme.txt
    \brief   description of the CMP PWM_signal_control demo

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

  This demo is based on the GD32150R-EVAL-V1.3 board, it shows how to control PWM 
output by using comparator output.

  CMP0 is configured as following:
  - Inverting input is internally connected to VREFINT = 1.2V.
  - Non Inverting input is connected to PA1.
  - Output is internally connected to TIMER0 OCPRECLR(output compare prepare clear).
  TIMER0 is configured as following:
  - TIMER clock frequency is set to systemcoreclock, 
  - Prescaler is set to 71 make the counter clock is 1MHz.
  - Counter autoreload value is 9999 and CH0 output compare value is 4999, TIMER0_CH0 outputs PWM with 50% dutycycle.
  - TIMER0_CH0 outputs PWM with 50% dutycycle.

  After system start-up, enable comparator and TIMER, then watch the waveform on PA8.
  While PA1 is lower than VREFINT(1.2V), PWM signal is displayed on PA8.
  While PA1 is higher than VREFINT, PWM signal is cleared and PA8 output low.
