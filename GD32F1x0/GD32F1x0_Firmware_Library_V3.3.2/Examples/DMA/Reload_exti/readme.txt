/*!
    \file    readme.txt
    \brief   description of DMA reload transfer number by EXTI interrupt in discontinuous mode

    \version 2020-07-16, V1.0.0, firmware for GD32F1x0
    \version 2020-09-21, V3.3.0, firmware update for GD32F1x0(x=3,5,7,9)
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

  This example is based on the GD32150R-EVAL-V1.3/GD32190R-EVAL-V1.2 board, it provides a 
description of how to reconfigure the DMA transfer number in discontinuous mode, when use 
DMA channel1 to transfer data from RAM memory to USART0 transmit data register(COM0 of 
GD32150R-EVAL-V1.3 board / COM1 of GD32190R-EVAL-V1.2 board, and JP14, JP15 on 
GD32190R-EVAL-V1.2 board must be fitted).

  The start of transfer is triggered by software. If the DMA transfer operation is finished 
correctly, data stored array welcome[] will be transfered to a serial port tool by USART0 
and LED1 light up. When the Tamper key is pressed, the DMA transfer number is reconfigured 
and DMA transmission is restarted, then the LED1 toggled.
