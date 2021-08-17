/*!
    \file  readme.txt
    \brief description of the USART transmit and receive interrupt
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to use the USART 
transmit and receive interrupts to communicate with the hyperterminal.

  Firstly, the USART sends string to the hyperterminal and still waits for receiving 
data from the hyperterminal. Only the number of data you enter is equal to or more 
than 32 bytes, USART will send string: "USART receive successfully!" (the reception 
is stopped when this maximum receive value is reached).
