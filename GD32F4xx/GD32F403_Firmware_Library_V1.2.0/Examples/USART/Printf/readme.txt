/*!
    \file  readme.txt
    \brief description of the USART printf
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to retarget the C library printf 
function to the USART.

  The USARTx is configured as follows:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One stop bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
