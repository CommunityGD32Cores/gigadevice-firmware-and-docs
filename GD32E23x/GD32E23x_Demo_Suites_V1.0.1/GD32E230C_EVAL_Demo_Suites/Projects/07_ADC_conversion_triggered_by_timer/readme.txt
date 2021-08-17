/*!
    \file  readme.txt
    \brief description of the ADC test demo
*/
    
    \version 2019-02-19, V1.0.0, firmware for GD32E230x
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

  This demo is based on the GD32E230C-EVAL-V1.1 board, it shows how to configure the 
ADC peripheral to convert the analog signal to digital data. PA2(channel2) is chosen 
as analog input pin. The ADC clock is configured to 12MHz. The CH0 event of TIMER0(the
frequence is 120kHz) is used as the trigger source of ADC. So the conversion frequence 
of ADC is also 120kHz.

  The converted data in the ADC_RDATA register is moved into SRAM(through DMA).

  The analog input pin PA2 should configured to analog mode. The users can change the
VR1 on the GD32E230C-EVAL-V1.1 board, and the LCD will display the ADC convert values 
with a voltage curve. The curve adjusts with the analog input.
