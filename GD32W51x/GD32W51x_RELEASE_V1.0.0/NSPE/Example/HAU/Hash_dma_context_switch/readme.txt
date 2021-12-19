/*!
    \file    readme.txt
    \brief   description of HASH context switch using DMA example

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

  This demo is based on the GD32W515P-EVAL-V1.0 board, it shows how to suspend a lower priority 
HASH calculationtask, and give priority to a higher priority task, after the higher priority task completed,
continue to handle the lower priority task.
  When power on, task 1 using HASH mode SHA-224 algorithm is processing. Multiple DMA is used to transfer
data from memory to the HAU processor. During the DMA transmission, a flag is used to trigger task 2 
insertion(normally it may be triggered by an interruption in user code). 
  Task 2 is using HASH mode SHA-256 algorithm. Single DMA mode is used to transfer data from memory
to the HAU processor.
  In this example, the message of task 1 to hash is 3567-bit data, the message of task 2 to hash 
is 261-bit data.
  The SHA-224 message digest result is a 224-bit data (28 bytes = 7 words) available in DO0...DO6
registers.
  The SHA-256 message digest result is a 256-bit data (32 bytes = 8 words) available in DO0...DO7 
registers.
  The all result data can be displayed on a PC HyperTerminal using the USART.
