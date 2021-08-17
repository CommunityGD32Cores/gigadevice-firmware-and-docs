/*!
    \file  readme.txt
    \brief description of the DMA flash to ram example
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it provides a description of how to 
use DMA0 channel0 to transfer data buffer from FLASH memory to embedded SRAM memory.
    
  Before programming the flash addresses, an erase operation is performed firstly.
After the erase operation, a comparison between FLASH memory and 0xFFFFFFFF(Reset value) 
is done to check that the FLASH memory has been correctly erased.

  Once the erase operation is finished correctly, the programming operation will be
performed by using the fmc_word_program function. The written data is transfered to 
embedded SRAM memory by DMA0 Channel0. The transfer is started by enabling the DMA0 Channel0.
At the end of the transfer, a Transfer Complete interrupt is generated since it
is enabled. A comparison between the FLASH memory and embedded SRAM memory is done to
check that all data have been correctly transferred.If the result of comparison is passed,
LED1 to LED4 light up. Otherwise LED1 and LED3 light up.
