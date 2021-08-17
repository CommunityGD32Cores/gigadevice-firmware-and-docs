/*!
    \file  readme.txt
    \brief description of SHA224_SHA256_dma example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the HAU
peripheral to hash data using SHA-224 and SHA-256 algorithms.
  For this example, DMA is used to transfer data from memory to the HAU processor.
The message to hash must be multiple of 512-bit (64 bytes) so the message must be 
padded to multiple of 64 bytes. In this example, the message to hash is 2040-bit 
(256 bytes) data.
  The SHA-224 message digest result is a 224-bit data (28 bytes = 7 words) available
in DO0...DO6 registers.
  The SHA-256 message digest result is a 256-bit data (32 bytes = 8 words) available
in DO0...DO7 registers.
  The all result data can be displayed on a PC HyperTerminal using the USART.
  JP5 must be jumpped to USART.  
