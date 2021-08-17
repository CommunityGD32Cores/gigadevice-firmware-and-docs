/*!
    \file  readme.txt
    \brief description of enet example

    \version 2014-12-26, V1.0.0, demo for GD32F10x
    \version 2017-06-20, V2.0.0, demo for GD32F10x
    \version 2018-07-31, V2.1.0, demo for GD32F10x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

  This demo is based on the GD32107C-EVAL-V1.3 board, it shows how to configure the 
enet peripherals to send and receive frames in normal mode and use lwip tcp/ip 
stack to realize ping, telnet and tcp server functions. 

  JP4,JP13,JP18 and JP19 must be fitted.

  It is configured in RMII mode, and 25MHz oscillator is used, the system clock 
is configured to 108MHz. Users should configure ip address, mask and gw of GD32107C-EVAL
board according to the actual net situation from the private defines in main.h.

  This demo realize three applications:
  1) Telnet application, the eval board acts as tcp server. Users can link 
the client with the eval board server, using 8000 port. Users can see the reply from the 
server, and can send the name(should input enter key) to server.

  2) tcp client application, the eval board acts as tcp client. Users can link 
the eval board client with the server, using 1026 port. The IP address of tcp server should 
be set in main.h by using marco of IP_S_ADDRx. Users can send information from server 
to client, then the client will send back the information. If the server is not online at first,
or is break during process, when the server is ready again,users can press tamper key to 
reconnect with server, and communicate.
 
  3) udp application. Users can link the eval board with another station, using 1025 port. 
The IP address of another station should be set in main.h by using marco of IP_S_ADDRx.
Users can send information from station to board, then the board will send back the information. 
  
  By default, the packet reception is polled in while(1). If users want to receive packet in 
interrupt service, uncomment the macro define USE_ENET_INTERRUPT in main.h.

  If users need dhcp function, it can be configured from the private defines in main.h.
This function is closed by default.

