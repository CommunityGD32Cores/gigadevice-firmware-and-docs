/*!
    \file  readme.txt
    \brief description of the auto baudrate detect
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to use the USART 
to detect and automatically set the USARTx_BAUD value based on the reception of 
one character.

  In this demo, firstly, the USART sends "\n\rUSART auto baudrate detection example\n\r"
to the hyperterminal, USART_ABDM_FTOF is set as the auto baud rate detection mode. If auto
baud rate detection is completed without error, LED1 is on.

   You can send any character be in a start 10xxxxxx frame format(eg.0x0a or 0x01)
to implement the auto baud rate detection.
