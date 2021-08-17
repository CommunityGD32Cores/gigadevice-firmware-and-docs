/*!
    \file  readme.txt
    \brief description of the USART DMA transmitter receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to use the USART
DMA transmit and receive.

  Firstly, the USART sends string to the hyperterminal and then loops waiting for receiving
max 10 datas from the hyperterminal. Every time if the number of data you enter is equal
to or more than 10 bytes, USART will send 10 bytes to the hyperterminal.
