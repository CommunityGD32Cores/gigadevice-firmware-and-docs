/*!
    \file  readme.txt
    \brief description of the enet demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to configure the 
enet peripherals to send and receive frames in normal mode and use lwip tcp/ip 
stack to realize ping, telnet and tcp server functions. 

  JP43, JP31, P2 must be fitted.

  It is configured in RMII mode, and 25MHz oscillator should be used, the system clock 
is configured to 108MHz. Users should configure ip address, mask and gw of GD32F30x-EVAL 
board according to the actual net situation from the private defines in main.h.

  This demo realize Telnet application, the eval board acts as tcp server. Users can link 
the client with the eval board server, using 23 port. Users can see the reply from the 
server, and can send the name(should input enter key) to server.

  By default, the packet reception is polled in while(1). If users want to receive packet in 
interrupt service, uncomment the macro define USE_ENET_INTERRUPT in main.h.

  If users need dhcp function, it can be configured from the private defines in lwipopts.h and main.h.
This function is closed in default.

