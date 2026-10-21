/*!
    \file    readme.txt
    \brief   description of the ADC analog watchdog

    \version 2023-12-31, V3.5.0, firmware update for GD32F1x0(x=3,5)
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

  This demo is based on the GD32150R-EVAL-V1.3 board, it shows how to use the ADC analog 
watchdog to guard continuously an ADC channel.The ADC is configured in continuous mode, 
PC1(GD32150R-EVAL-V1.3) is chosen as analog input pin.The ADC clock is configured to 
12MHz.

  Change the VR1 on the board, when the channel11(channel10) converted value is over 
the programmed analog watchdog high threshold (value 0x0A00) or below the analog 
watchdog low threshold(value 0x0400),an WDE interrupt will occur, and LED2 will turn on.  
When the channel11 converted value is in safe range(among 0x0400 and
0x0A00), the LED2 will be off.

  The analog input pin should configured to AIN mode and the ADC clock should be 
below 14MHz for GD32150R.
