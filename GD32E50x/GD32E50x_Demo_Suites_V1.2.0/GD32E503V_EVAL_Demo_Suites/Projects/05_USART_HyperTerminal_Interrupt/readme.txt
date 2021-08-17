/*!
    \file    readme.txt
    \brief   USART transmit and receive interrupt

    \version 2020-09-04, V1.0.0, demo for GD32E50x
    \version 2021-03-31, V1.1.0, demo for GD32E50x
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

  This demo is based on the GD32E503V-EVAL-V1.0 board, it shows how to use USART0 transmit 
and receive interrupts to communicate with the serial terminal tool supporting hex
format communication.
  Firstly, the USART0 sends the strings to the hyperterminal and still waiting for 
receiving data from the hyperterminal. Then, compare tx_buffer with the rx_buffer,
if the tx_buffer is the same with the rx_buffer, LED1, LED2, LED3, LED4 light by turns. Otherwise, 
LED1, LED2, LED3, LED4 toggle together.

The USARTx is configured as follow:
  - baudrate = 115200 baud  
  - word length = 8 Bits
  - one stop bit
  - no parity
  - hardware flow control disabled (RTS and CTS signals)
  - receive and transmit enabled
