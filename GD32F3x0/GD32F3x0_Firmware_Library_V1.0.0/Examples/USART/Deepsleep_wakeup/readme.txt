/*!
    \file  readme.txt
    \brief description of the USART wakeup from deepsleep
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to use the USART
to wake up the MCU from deepsleep mode.

  In this demo, the wake up method is configured as start bit detection. When the MCU
enters into deepsleep mode, the LED2 stops in a certain status(on or off). In this 
case, you can send some characters to wake up USART and the LED2 blinks.

