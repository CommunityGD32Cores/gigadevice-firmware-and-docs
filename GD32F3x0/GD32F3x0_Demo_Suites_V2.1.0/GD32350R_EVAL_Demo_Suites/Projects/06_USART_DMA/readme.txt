/*!
    \file    readme.txt
    \brief   description of USART DMA transmitter receiver example
    
    \version 2017-06-28, V1.0.0, demo for GD32F3x0
    \version 2019-06-01, V2.0.0, demo for GD32F3x0
    \version 2021-05-31, V2.1.0, demo for GD32F3x0
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

  This demo is based on the GD32350R-EVAL-V1.0 board, it shows how to use the USART transmit 
and receive data in DMA mode via hyperterminal.
  Firstly, all the LEDs are turned on and off for test. Then, the USART transmits the strings
to the hyperterminal and waits for receiving data from the hyperterminal. Note that the data 
transmitted and received in hyperterminal are in hexadecimal format. After that, compare tx_buffer
with rx_buffer. If tx_buffer is same with rx_buffer, LED1 and LED2 are turned on, LED3 and LED4
are turned off. Otherwise, LED1 and LED2 are turned off, LED3 and LED4 are turned on.

  Jump JP13 to USART
