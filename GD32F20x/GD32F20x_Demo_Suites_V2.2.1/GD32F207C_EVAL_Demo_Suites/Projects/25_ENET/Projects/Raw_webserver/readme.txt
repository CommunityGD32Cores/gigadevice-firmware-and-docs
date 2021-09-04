/*!
    \file  readme.txt
    \brief description of the enet demo

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

  This demo is based on the GD32207C-EVAL-V1.2 board, it shows how to configure the 
enet peripherals to send and receive frames in normal mode and use lwip tcp/ip 
stack to realize webserver application. 

  JP4,JP13 and JP18 must be fitted.

  It is configured in RMII mode, and 25MHz oscillator is used, the system clock 
is configured to 120MHz. Users should configure ip address, mask and gw of GD32207C-EVAL-V1.2 
board according to the actual net situation from the private defines in main.h.

  This demo realize webserver application:
  Users can visit the eval board through Internet Explorer, the eval board acts as a webserver,
and the url is the local ip address of the eval board. There are two experiments realized, one is 
the LEDs control, the other one is the ADC monitoring Vref voltage in real-time.
  If users need dhcp function, it can be configured from the private defines in main.h. This 
function is closed by default. Users can use a router to connect the eval board, and use the HyperTerminal
to print the automatic allocated ip address, then connect your mobile phone to the wifi which
is the router send. Users can visit the eval board and control it on your mobile phone.
 
  By default, the packet reception is polled in while(1). If users want to receive packet in 
interrupt service, uncomment the macro define USE_ENET_INTERRUPT in main.h.

  

