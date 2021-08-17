/*!
    \file  readme.txt
    \brief description of the EXMC_NAND demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use EXMC 
peripheral to access NAND memory. This board uses nand flash controller of
EXMC module to control bank1, which supports NAND memory. 
  This demo shows the write and read data operation process of NAND memory by 
EXMC module. Firstly read NAND_ID. If read ID failure, print out the failure 
prompt information. Otherwise write and read NAND memory. If the access correctly, 
LED1 will be turned on and print out the information to access to NAND memory 
successfully. Otherwise, LED4 are turned on print out the information to access 
to NAND memory failure.
  Put jumper "JP5" to "USART1" to connect the serial port to COM1£¬the operation 
process can be view via the serial debugging assistant.