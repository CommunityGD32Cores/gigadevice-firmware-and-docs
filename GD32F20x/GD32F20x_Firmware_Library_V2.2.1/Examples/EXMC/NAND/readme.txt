/*!
    \file    readme.txt
    \brief   description of the EXMC_NAND demo

    \version 2015-07-15, V1.0.0, firmware for GD32F20x
    \version 2017-06-05, V2.0.0, firmware for GD32F20x
    \version 2018-10-31, V2.1.0, firmware for GD32F20x
    \version 2020-09-30, V2.2.0, firmware for GD32F20x
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

  This demo is based on the GD32207i-EVAL-V1.2 board, it shows how to use EXMC 
peripheral to access NAND memory. This board uses nand flash controller of
EXMC module to control bank1, which supports NAND memory. 

  This demo shows the write and read data operation process of NAND memory by 
EXMC module. Firstly read NAND_ID. If read ID failure, print out the failure 
prompt information. Otherwise write and read NAND memory. If the access correctly, 
LED2 will be turned on and print out the information to access to NAND memory 
successfully. Otherwise, LED4 are turned on print out the information to access 
to NAND memory failure. P2,P3,JP24 must be fitted.

  Put jumper "JP5" to "USART1" to connect the serial port to COM1£¬the operation 
process can be view via the serial debugging assistant.