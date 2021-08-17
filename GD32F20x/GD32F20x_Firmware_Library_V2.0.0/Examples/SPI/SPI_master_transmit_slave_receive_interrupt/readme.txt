/*!
    \file  readme.txt
    \brief description of the master transmit and slave receive through interrupt demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32F207i-EVAL board, it shows master send data and 
slave receive data use intterupt mode.
  After the communicate is complete,if receive data equal to send data,led1 turn 
on,if not led1 turn off.

  Connect SPI0 SCK  PIN(PA5) to SPI2 SCK  PIN(PC10).
  Connect SPI0 MOSI PIN(PA7) to SPI2 MISO PIN(PC11).
