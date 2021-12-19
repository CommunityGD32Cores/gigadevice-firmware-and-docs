/*!
    \file    readme.txt
    \brief   description of the master and slave fullduplex communication use DMA demo
    
    \version 2021-08-04, V1.0.0, firmware for GD32L23x
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

  This example is based on the GD32L233R-EVAL-V1.0 board, it shows a fullduplex
communication between SPI0 and SPI0 using DMA mode on two boards. 
  Firstly, modify file: examples/SPI/SPI_master_slave_fullduplex_dma/main.c to choose 
master or slave.
  For example, choose master mode:
  #define MASTER
  //#define SLAVE
  Then, download the code of the master or slave to the corresponding development board.
  After the communication is complete, if the receive data equals to the send data, 
LED1 on two boards turn on, if not, LED1 on two boards turn off.

Test steps:
  1. press slave board reset key
  2. press master board reset key
  
  Connect SPI0 NSS  PIN(PA4) TO SPI1 NSS  PIN(PB12).
  Connect SPI0 SCK  PIN(PA5) TO SPI1 SCK  PIN(PB13).
  Connect SPI0 MISO PIN(PA6) TO SPI1 MISO PIN(PB14).
  Connect SPI0 MOSI PIN(PA7) TO SPI1 MOSI PIN(PB15).