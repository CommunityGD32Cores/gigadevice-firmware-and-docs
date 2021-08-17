/*!
    \file  readme.txt
    \brief description of the master transmit and slave receive through interrupt demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it shows master send data and 
slave receive data use intterupt mode.
  After the communicate is complete,if receive data is equal to send data,led1 turns 
on,if not led1 turns off.

  Connect SPI0 SCK  PIN(PA5) to SPI2 SCK  PIN(PC10).
  Connect SPI0 MOSI PIN(PA7) to SPI2 MISO PIN(PC11).

  JP12 and JP13 must be jumpped to SPI.