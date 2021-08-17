/*!
    \file  readme.txt
    \brief description of ADC0_ADC1_Follow_up_mode example
    
    \version 2017-02-10, V1.0.0, demo for GD32F403
    \version 2018-10-10, V1.1.0, demo for GD32F403
    \version 2018-12-25, V2.0.0, demo for GD32F403
/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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
  This demo is based on the GD32303C-EVAL-V1.1 board, it shows how to use ADC0 and ADC1 regular_follow-up 
mode convert function. PC3 and PC5 are configured as AIN mode. PC5 pin connect to the external voltage 
input. PC3 is the output voltage of the slide rheostat VR1 on board.
  TIMER0_CH0 is the trigger source of ADC0. When the rising edge of TIMER0_CH0 coming, ADC0 starts immediately
and ADC1 starts after a delay of several ADC clock cycles. The values of ADC0 and ADC1 are transmitted to array 
adc_value[] by DMA.
  We can watch array adc_value[] by COM0.

  Keep PC5 pin should not be reused by other peripherals.

