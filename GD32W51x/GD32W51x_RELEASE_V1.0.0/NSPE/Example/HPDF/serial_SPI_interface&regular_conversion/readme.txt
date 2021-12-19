/*!
    \file    readme.txt
    \brief   description of serial SPI interface and regular conversion
    
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

  This demo is based on the GD32W515P-EVAL-V1.0 board, it shows the HPDF sample stream data 
by serial SPI interface. In this demo, configure channel1 is serial channel to recieve the 
data stream. The SPI0 is used as sigma-delta modulator to transmit stream data and provide
external clock.The CKIN0 pin is connect PC0, the DATA0 pin is connect PC3.

  This demo shows process of the HPDF recieve and smooth the SPI serial data stream. If the 
HPDF output data is exceed the threshold monitor high/low threshold or malfunction monitor 
counter, the error information will be print. If the HPDF output data is not exceed the 
threshold, the HPDF output data will be print by COM0.

  Connect SPI0 SCK  PIN(PA5) to HPDF CKIN0  PIN(PC0).
  Connect SPI0 MOSI PIN(PA7) to HPDF DATAIN0 PIN(PC3).
