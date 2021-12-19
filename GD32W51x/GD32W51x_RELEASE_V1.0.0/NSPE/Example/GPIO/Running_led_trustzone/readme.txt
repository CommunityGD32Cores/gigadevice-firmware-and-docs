/*!
    \file    readme.txt
    \brief   description of running LED example with TZEN = 1

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
two sub-projects, secure project and non-secure project. It shows how to switch from 
secure application to non-secure application, and call secure functions in nonsecure
application. The led_nonsecure_cfg function is defined in non-secure callable area in 
secure application, and is called by nonsecure application. 
  The system always starts from the secure application, and the secure application is
responsible for starting the nonsecure application. Therefore, in the compilation process,
first compile the secure project, and then compile the non-secure project
  In secure application, the program execution follows the following steps:
    1.configure SAU/IDAU, FPU and secure/nosecure interrupts allocations in partition_gd32w51x.h file
    the key configuration is as follows:
        Enable SAU
        SAU Region 0  start address: 0x0C03E000   end address: 0xC003FFFF  Attributes: S/NSC
        SAU Region 1  start address: 0x08040000   end address: 0x081FFFFF  Attributes: NS
        SAU Region 2  start address: 0x20010000   end address: 0x2001FFFF  Attributes: NS
        SAU Region 3  start address: 0x40000000   end address: 0x4FFFFFFF  Attributes: NS
    2.enable trustzone
        TZEN = 1
    3.configure secure mark pages 
        startpage :0x00  endpage:0x3F  means that the first 64 pages of flash are configured as secure
    4.configure TZBMPC
        TZBMPC1 vector register y(y=0 to 7):0x00000000 means that SRAM1(64KB) are configured as nonsecure
    5.Setup and jump to non-secure application

Non-secure Application :
  In non-secure application, code will first call NSC function led_nonsecure_cfg to configure LED pin to
nonsecure, then LED1 on, LED2 on, three LEDs can light periodically. 
  On the GD32W515P-EVAL-V1.0 board, LED1 connected to PB6, LED2 connected to PA15,LED3 connected to PA8.
 