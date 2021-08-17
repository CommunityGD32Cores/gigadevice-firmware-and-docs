/*!
    \file  readme.txt
    \brief description of the master and slave fullduplex communication use polling demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32F207i-EVAL board, it shows SPI0 and SPI2 fullduplex
communication use polling mode.After the communicate is complete,if receive data equal 
to send data, led1 and led2 turn on, if not led1 and led2 turn off.

  Connect SPI0 SCK  PIN(PA5) TO SPI2 SCK  PIN(PC10).
  Connect SPI0 MISO PIN(PA6) TO SPI2 MISO PIN(PC11).
  Connect SPI0 MOSI PIN(PA7) TO SPI2 MOSI PIN(PC12). 