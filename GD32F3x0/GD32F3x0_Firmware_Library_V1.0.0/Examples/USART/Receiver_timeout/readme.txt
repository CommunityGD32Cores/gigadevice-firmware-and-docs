/*!
    \file  readme.txt
    \brief description of the UASRT receiver timeout
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL, it shows how to use the USART receiver 
timeout function.

  Firstly, the USART sends "a usart receive timeout test example!" to the hyperterminal
and then loops waiting for receiving data from the hyperterminal. If you do not enter any
data more than 3 seconds, USART will send string to the hyperterminal what you have entered
and start a new round of waiting.
