/*!
    \file  readme.txt
    \brief description of TRNG_poll_mode example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the TRNG 
peripheral to generate random 32bit numbers. After the system start-up, 
LED1 is on. if application initialize the TRNG module successfully, it will 
print success message using USART to PC HyperTerminal, otherwise it will 
print error message. When Tamper Key is pressed, application trys to get the 
random 32bit number and print the data to PC HyperTerminal.
  JP5 must be jumpped to USART.

