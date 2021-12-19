/*!
    \file    readme.txt
    \brief   description of DMA request synchronization example

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
  
  This example is based on the GD32L233R-EVAL-V1.0 board, it provides a description 
of how to use DMAMUX request multiplexer channel 0 synchronization mode for DMA 
transmission.
  In this demo, DMA channel 0 is configured to transfer data from memory to perpheral. 
DMAMUX request multiplexer channel 0 is configured as synchronization mode, using EXTI_0
line rising edge to start 4 DMA requests transfering to DMA controller.
  The steps are shown as follows:
  1. At the start, users press the Wakeup key to start 4 DMA requests transfering. The DMA
     controller will transfer data from memory to USART1 TX register. 
  2. Repeat several times to press the Wakeup key to start another 4 DMA requests transfering 
     until all the data are transfered to USART1 TX register. When all data are transfered,
     a DMA transfer complete interrupt will occur. 
  3. If the transfer is successful, LED1 will be on.
