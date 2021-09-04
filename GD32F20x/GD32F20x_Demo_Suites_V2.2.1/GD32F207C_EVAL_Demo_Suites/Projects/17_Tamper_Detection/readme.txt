/*!
    \file  readme.txt
    \brief description of TAMPER demo

    \version 2015-07-15, V1.0.0, demo for GD32F20x
    \version 2017-06-05, V2.0.0, demo for GD32F20x
    \version 2018-10-31, V2.1.0, demo for GD32F20x
    \version 2020-09-30, V2.2.0, demo for GD32F20x
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

  This demo is based on the GD32207C-EVAL-1.2 board, it shows how to write/read data 
to/from Backup data registers and demonstrates the TAMPER function. In this demo, 
the first TAMPER detection is enabled. 
  After system start-up, it writes the data to all Backup data registers, then check
whether the data were correctly written. If data written correctly, LED2 is on, 
otherwise LED3 is on.When the tamper key(TAMPER pin) is pressed, the backup data 
registers are reset and the tamper interrupt is generated. In the corresponding ISR, 
it checks whether the backup data registers are cleared or not. If backup data registers 
are cleared, LED4 is on, otherwise LED5 is on.

  TAMPER and Waveform Detection only one can be used at the same time.