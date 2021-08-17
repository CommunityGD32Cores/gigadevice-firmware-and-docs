
/*!
    \file  readme.txt
    \brief description of the USART HalfDuplex
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows a basic communication between USART0 and USART1 in Half-Duplex mode.
  The transmitter_buffer0 and transmitter_buffer1 are sent by USART0 and USART1 respectively and 
displayed in HyperTerminal. If the data received by USART0 is equal to transmitter_buffer1, LED1 
is turned on. If the data received by USART1 is equal to transmitter_buffer0, LED2 is turned on.

  connect  USART0_Tx(PA9) to USART1_Tx(PA2)
