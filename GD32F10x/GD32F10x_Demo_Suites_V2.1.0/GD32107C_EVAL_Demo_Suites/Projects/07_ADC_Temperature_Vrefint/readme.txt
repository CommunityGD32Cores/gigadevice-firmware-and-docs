/*!
    \file  readme.txt
    \brief description of ADC_Temperature_Vrefint example
    
    \version 2014-12-26, V1.0.0, demo for GD32F10x
    \version 2017-06-30, V2.0.0, demo for GD32F10x
    \version 2018-07-31, V2.1.0, demo for GD32F10x
*/

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

  This demo is based on the GD32107C-EVAL-V1.3 board, it shows how to use 
the ADC to convert analog signal to digital data. The ADC is configured 
in dependent mode, inner channel 16(temperature sensor channel) and channel 17
(VREFINT channel) are chosen as analog input pin.

  As the AD convertion begins by software, the converted data is stored in the 
ADC_IDATAx register, where the x is 0 to 1. USART will print these values. Connect
serial cable to EVAL_COM0 and open the HyperTerminal, and jump the JP5 to USART.

Notice: Because there is an offset, when inner temperature sensor is used to detect 
accurate temperature, an external temperature sensor part should be used to calibrate 
the offset error.

