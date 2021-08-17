/*!
    \file  readme.txt
    \brief description of SHA1_MD5_dma example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the HAU
peripheral to hash data using SHA-1 and MD5 Algorithms.
  For this example, DMA is used to transfer data from memory to the HAU processor.
The message to hash is a 2040 bit data, the SHA-1 message digest result is a 160 
bit data and the MD5 message digest result is a 128 bit data.
  The all result data can be displayed on a PC HyperTerminal using the USART.
  JP5 must be jumpped to USART.