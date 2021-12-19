/*!
    \file    readme.txt
    \brief   description of Hash SHA1 dma example with TZEN=1

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

  This example is based on the GD32W515P-EVAL-V1.0 board, the project consists of 
two sub-projects, secure project and nonsecure project. It shows how to switch between 
secure application and non-secure application. 
  The hash_sha1_dma and callback_config function is defined in non-secure callable area 
in secure application, and is called by nonsecure application.
  The secure_fault and secure_error function is defiend in nonsecure application, and is 
called by secure application.
  The system always starts from the secure application, and the secure application is 
responsible for starting the nonsecure application.

  In secure application,the program execution follows the following steps:
    1.configre SAU/IDAU, FPU and secure/nosecure interrupts allocations in 
    partition_gd32w51x.h file the key configuration is as follows:
        Enable SAU
        SAU Region 0  start address: 0x0C03E000  end address: 0x0C03FFFF  Attributes: S/NSC
        SAU Region 1  start address: 0x08040000  end address: 0x08200000  Attributes: NS
        SAU Region 2  start address: 0x20010000  end address: 0x2001FFFF  Attributes: NS
        SAU Region 3  start address: 0x40000000  end address: 0x4FFFFFFF  Attributes: NS
    2.configure clocks    
    3.configure TZBMPC
        TZBMPC1 vector register y(y=0 to 7):0x00000000 means that SRAM1(64KB) are configured
        as nonsecure
    4.Configure gpio of LEDs as nonsecure atrribute.        
    5.enable illegal access interrupt and SecureFault interrupt and configure NVIC    
    6.configure and jump to non-secure application
 
 In nonsecure application, by calling NSC area functions hash_sha1_dma and callback_config,
the Hash SHA1 using DMA is calculated, if the computed result is right, LED1 is on, otherwise
LED2 is on. If illegal access interrupt or SecureFault interrupt occurs, LED2 will be on.
