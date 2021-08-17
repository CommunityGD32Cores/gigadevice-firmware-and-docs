/*!
    \file  readme.txt
    \brief description of CAU_AESECB_mode example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the CAU 
peripheral to encrypt and decrypt data using AES-128 Algorithm.
  For this example, DMA is used to transfer data from memory to the CAU processor
IN FIFO and also to transfer data from CAU processor OUT FIFO to memory.
  The data to be encrypted is a 256-bit data (8 words), which corresponds to 
2x AES-128 Blocks.
  The all data can be displayed on a PC HyperTerminal using the USART.
    JP5 must be jumpped to USART.
