/*!
    \file  readme.txt
    \brief description of the Erase_Program example
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it provides a description of 
how to erase and program flash memory.
 
  In erasing operation, a comparison between flash memory and 0xffffffff is done to 
check whether the flash memory has been correctly erased. If the result is wrong, 
LED1 will be on.

  In programming operation, a comparison between flash memory and target data is 
done to check whether the flash memory has been correctly programmed. If the result 
is wrong, LED2 will be on.
 
  If all the three operations are successful, LED3 will be on.
