
/*!
    \file  readme.txt
    \brief description of the USART HalfDuplex
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it provides a basic communication
USART0 with USART1 in Half-Duplex mode.

  The transmitter_buffer0 and transmitter_buffer1 are sent by USART0 and USART1 respectively and 
displayed in HyperTerminal. If the data received by USART0 is equal to transmitter_buffer1 and the
data received by USART1 is equal to transmitter_buffer0, LED1 and LED2 is turned on.

  connect USART0_Tx(PA9) to USART1_Tx(PA2)


