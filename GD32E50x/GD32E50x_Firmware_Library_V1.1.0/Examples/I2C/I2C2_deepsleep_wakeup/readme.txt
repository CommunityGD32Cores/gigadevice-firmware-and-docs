/*!
    \file    readme.txt
    \brief   description of wakeup from deepsleep

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
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

  This demo is based on the GD32E507Z-EVAL-V1.0, it shows how to wakeup pmu from deepsleep 
mode by I2C address match. Select the IRC8M as the clock of I2C2. Press key A to 
enter deepsleep mode, led stop flashing. When the I2C received address that matches 
with its address, mcu will be wakeuped from deepsleep mode, the transfer goes on
normally and led sparks again. But the led sparks slower, because at this time IRC8M 
is the system clock.
  In this demo, I2C2 is operated as slave receiver, and the SCL line and SDA line of 
I2C2 interface are controlled by the I/O pin PA8 and PC9 respectively. 
  PA8 and PC9 should connect with a pull-up resistor.


