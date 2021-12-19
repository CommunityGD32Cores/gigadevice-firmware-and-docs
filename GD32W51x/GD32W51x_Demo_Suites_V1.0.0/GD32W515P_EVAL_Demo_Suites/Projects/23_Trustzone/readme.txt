/*!
    \file    readme.txt
    \brief   description of Trustzone demo

    \version 2021-01-21, V1.0.0, demo for GD32W51x
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

  This example is based on the GD32W515P-EVAL-V1.1 board, the project consists of 
two sub-projects, secure project and non-secure project. It shows how to switch from 
secure application to non-secure application, and call secure functions in non-secure
application. The entry_cb_func_register and non_secure_print functions are defined in non-secure callable area in 
secure application, and is called by nonsecure application. The nonsecure_func is the function address of toggle_led1
in non-secure application, which is return by entry_cb_func_register.
  The system always starts from the secure application, and the secure application is the function 
responsible for starting the non-secure application. Therefore, in the compilation process,
first compile the secure project, and then compile the non-secure project
  In secure application, the program execution follows the following steps:
    1.configure SAU/IDAU, FPU and secure/nosecure interrupts allocations in partition_gd32w51x.h file
    the key configuration is as follows:
        Enable SAU
        SAU Region 0  start address: 0x0C03E000   end address: 0xC003FFFF  Attributes: S/NSC
        SAU Region 1  start address: 0x08040000   end address: 0x081FFFFF  Attributes: NS
        SAU Region 2  start address: 0x20010000   end address: 0x2001FFFF  Attributes: NS
        SAU Region 3  start address: 0x40000000   end address: 0x4FFFFFFF  Attributes: NS
    2.configure secure mark pages 
        startpage :0x00  endpage:0x3F  means that the first 64 pages of flash are configured as secure
    3.enable trustzone
        TZEN = 1
    4.configure TZBMPC
        TZBMPC1 vector register y(y=0 to 7):0x00000000 means that SRAM1(64KB) are configured as nonsecure
    5.configure LED1 and LED2 to non-secure aceess
    6.set COM0 port to secure aceess and configure it
    7.Setup and jump to non-secure application
  Secure application also calls nonsecure_func to toggle LED1 and prints information through COM0 periodically in SysTick_Handler.

Non-secure Application :
  In non-secure application, code will first call NSC function entry_cb_func_register to configure nonsecure_func, then secure
application can use nonsecure_func to call toggle_led1 in non-secure application, non-secure code will toggle LED2 and prints
information through COM0 periodically. Although COM0 is secure aceess only, but non-secure code can call NSC function non_secure_print
to print.
  On the GD32W515P-EVAL-V1.1 board, LED1 connected to PB6, LED2 connected to PA15, COM0 is used USART2.