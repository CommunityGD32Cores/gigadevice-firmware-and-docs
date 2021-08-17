/*!
    \file  readme.txt
    \brief description of the EXMC_NOR demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to use EXMC peripheral to control NOR memory. It describes 
the write and read data operation process of NOR memory by EXMC module. If the 
operation is correct, this demo will turn on LED1 and print out a success information 
and the data writed to the NOR memory. Otherwise, LED4 is turned on and a failure 
information will be printed out. Whether the operation is successful or not, the data read 
from NOR flash will be printed out.
  D0-D15,A0-A22,NWE,NOE and NE1 must be fitted.
