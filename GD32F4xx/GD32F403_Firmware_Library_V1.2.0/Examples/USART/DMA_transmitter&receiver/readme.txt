/*!
    \file  main.c
    \brief description of the USART DMA transmitter receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to use the USART DMA transmit and receive.
  Firstly, the USART sends the strings to the hyperterminal and then loop waiting for
receiving max 10 bytes data from the hyperterminal. Every time if the number of data you 
enter equal with or more than 10 bytes, USART will send 10 bytes to the hyperterminal.
