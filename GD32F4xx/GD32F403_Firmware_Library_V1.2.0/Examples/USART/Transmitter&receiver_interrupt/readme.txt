/*!
    \file  readme.txt
    \brief description of the USART transmit and receive interrupt
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to use the USART transmit and receive interrupts to communicate 
with the hyperterminal.
  Firstly, the USART sends the strings to the hyperterminal and still waits for
receiving data from the hyperterminal. Only the number of data you enter equals with
or more than 32 bytes, USART will send strings: "USART receive successfully!" (the 
reception is stopped when this maximum receive value is reached).

