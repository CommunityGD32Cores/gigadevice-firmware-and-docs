/*!
    \file  readme.txt
    \brief description of the master and slave fullduplex communication use polling demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it shows SPI0 and SPI2 fullduplex
communication use polling mode.After the communicate is complete,if receive data is equal 
to send data, led1 and led2 turn on, if not led1 and led2 turn off.

  Connect SPI0 SCK  PIN(PA5) TO SPI2 SCK  PIN(PC10).
  Connect SPI0 MISO PIN(PA6) TO SPI2 MISO PIN(PC11).
  Connect SPI0 MOSI PIN(PA7) TO SPI2 MOSI PIN(PC12). 
  
  JP12 and JP13 must be jumpped to SPI.