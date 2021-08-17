/*!
    \file  readme.txt
    \brief description of the UASRT receiver timeout
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL, it shows how to use the USART 
receiver timeout function.  
  Firstly, the USART sends "a usart receive timeout test example!" to the hyperterminal
and then loop waiting for receiving data from the hyperterminal. Then send data from the 
hyperterminal to USART. If you did not send any data more than 3 seconds, USART will send 
strings to the hyperterminal what you have sent and start a new round of waiting.