/*!
    \file  readme.txt
    \brief description of SD card read and write example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32207i-EVAL board, it shows how to use SDIO to read or
write to SD card. After initializing the card successfully, print out the detailed information
of the card by USART. Then write a block of data to the card and read. If any error occurs,
print the error message and turn on LED1, LED3 and turn off LED2, LED4. After that, do the
lock and unlock operation test. Lock the card first and try to erase the data of the card. 
Then unlock the card and erase the card. If any error occurs, print the error message and 
turn on LED1, LED3 and turn off LED2, LED4. Last is the multiple blocks operation test. If
no error occurs, turn on all the LEDs.

  Uncomment the macro DATA_PRINT to print out the data and display them through HyperTerminal.

  Jump JP5 to USART
  Jump JP26,JP28,JP29 and JP31 to SDIO