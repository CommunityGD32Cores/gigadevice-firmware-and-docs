/*!
    \file    readme.txt
    \brief   description of write_protection example
    
    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0
    \version 2020-09-30, V2.1.0, firmware for GD32F3x0
    \version 2022-01-06, V2.2.0, firmware for GD32F3x0
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

  This example is based on the GD32350R-EVAL-V1.0 board, it provides a description 
of how to enable and disable the write protection for the embedded flash.
    
  After system start-up, LED1 and LED4 usually are light on. Meanwhile, LED2 and LED3 usually 
are off. It indicates that no pages of flash are in write protection and all pages can be 
erased/programmed successfully. If not, press the key User to change the board to the normal 
condition.
    
  STEP1: Press the key Wakeup. The LED1 and LED4 will be off and LED2 and LED3 will be on. 
It indicates that some pages are in write protection and erasing/programming on them is failed.

  STEP2: Press the key User. The LED1 and LED4 will be on and LED2 and LED3 will be off. 
It indicates that those pages are out of write protection and erasing/programming on them is successful.

  STEP3: Press the key Tamper. The LED2 and LED4 will be on and LED1 and LED3 will be off. 
It indicates that some pages are out of write protection and can be erased/programmed successfully.
But some other pages are still in write protection.

  STEP4: Press the key User. The LED1 and LED4 will be on and LED2 and LED3 will be off. 
It indicates that all pages are out of write protection and erasing/programming on them is successful.

  Note: After testing the example, please ensure to press the key User to disable all pages' 
write protection.


