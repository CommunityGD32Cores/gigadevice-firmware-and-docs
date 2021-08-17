/*!
    \file  readme.txt
    \brief description of the EXMC_NAND demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to use EXMC peripheral to control NAND memory. This board 
uses EXMC_BANK1_NAND to support NAND memory. 
  This demo shows the write and read data operation process of NAND memory by 
EXMC module. Firstly, read NAND_ID. If the operation fails, this demo will print
out the failure information and light on LED4, otherwise print out the ID information. 
Secondly, write and read the NAND memory. If the test result is correct, LED1 will be 
turned on and the data writed to the NAND will be printed out.
  JP36(nwait),D0-D7,nwait,A16(CLE),A17(ALE),NWAIT,NWE,NOE and NCE1 must be fitted. 
