/*!
    \file  readme.txt
    \brief description of program_erase example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it provides a
description of how to erase and program flash memory and reprogram without previous
earse.
 
  In erasing operation, a comparison between flash memory and 0xffffffff is done to 
check whether the flash memory has been correctly erased. If the result is wrong, 
LED1 will be on.

  In programming operation, a comparison between flash memory and target data is 
done to check whether the flash memory has been correctly programmed. If the result 
is wrong, LED2 will be on.

  In reprogramming operation, corresponding bit value will be changed from 1 to 0, 
If the result is wrong, LED3 will be on.
  
  If all the three operations are successful, LED4 will be on.
 