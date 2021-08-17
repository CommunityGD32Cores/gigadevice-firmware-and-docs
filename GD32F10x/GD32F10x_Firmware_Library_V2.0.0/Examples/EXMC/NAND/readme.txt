/*!
    \file  readme.txt
    \brief description of the EXMC_NAND demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to use EXMC 
peripheral to control NAND memory. This board uses EXMC_BANK1_NAND to support 
NAND memory.

  This demo shows the write and read data operation process of NAND memory by 
EXMC module. Firstly, read NAND_ID. If the operation fails, this demo will print
out the failure information and light on LED4, otherwise print out the ID information. 
Secondly, write and read the NAND memory. If the test result is correct, LED1 will be 
turned on and the data writed to the NAND will be printed out.

  JP24(Nand),JP5, JP6, P2 and P3 must be fitted. 
