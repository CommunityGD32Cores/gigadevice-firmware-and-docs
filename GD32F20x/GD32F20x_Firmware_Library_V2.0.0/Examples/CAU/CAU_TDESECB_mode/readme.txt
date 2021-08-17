/*!
    \file  readme.txt
    \brief description of CAU_TDESECB_mode example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the CAU
peripheral to encrypt and decrypt data using TDES algorithm.
  For this example, DMA is used to transfer data from memory to the CAU processor
IN FIFO and also to transfer data from CAU processor OUT FIFO to memory.
  The data to be encrypted is a 320 bit data, which corresponds to 5 TDES Blocks.
The all data can be displayed on a PC HyperTerminal using the USART.
  JP5 must be jumpped to USART.
