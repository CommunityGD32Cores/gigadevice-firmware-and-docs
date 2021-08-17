/*!
    \file  readme.txt
    \brief description of the master transmit and slave receive through interrupt demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows master send data and 
slave receive data use intterupt mode.After the communicate is complete,if receive data
equal to send data,led1 turn on,if not led1 turn off.

  Connect SPI0 SCK  PIN(PA5) TO SPI1 SCK  PIN(PB13)
  Connect SPI0 MOSI PIN(PA7) TO SPI1 MOSI PIN(PB14)
