/*!
    \file  main.c
    \brief description of the USART DMA transmitter receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the USART
DMA transmit and receive.
  Firstly, the USART sends the strings to the hyperterminal and then loops waiting for
receiving max 10 bytes data from the hyperterminal. Every time if the number of data you 
enter is equal to or more than 10 bytes, USART will send 10 bytes to the hyperterminal.
