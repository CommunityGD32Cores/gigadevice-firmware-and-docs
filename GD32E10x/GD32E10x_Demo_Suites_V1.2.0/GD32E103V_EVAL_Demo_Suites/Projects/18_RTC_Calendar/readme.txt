/*!
    \file    readme.txt
    \brief   description of calendar demo
    
    \version 2018-03-26, V1.0.0, demo for GD32E103
    \version 2020-09-30, V1.1.0, demo for GD32E103
    \version 2020-12-31, V1.2.0, demo for GD32E103
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

  This demo is based on the GD32E103V-EVAL-V2.0 board, it explains how to configure 
the RTC module. In this demo, RTC peripheral is configured to display calendar.

  Select LXTAL as RTC clock source. And the HyperTerminal should be connected to the 
evaluation board via COM1.

  After start-up, the program checks whether the BKP data register is written in a 
key value or not. 

  The register has not yet been programmed when the program is executed for the first 
time. At this time, the RTC will be configured. The user will be asked to set the date 
and time on the HyperTerminal. And the calendar will be displayed on the LCD.

  If a power on reset or an external reset occurs, and the BKP domain is not reset. 
RTC does not need to be configured and the value of the calendar will be continued with 
the value before reset.

  Press the USER key, all LEDs will be toggle after 10s.

  P2 and P3 must be fitted to the EXMC port.

  JP23 must be installed a LCD device.
