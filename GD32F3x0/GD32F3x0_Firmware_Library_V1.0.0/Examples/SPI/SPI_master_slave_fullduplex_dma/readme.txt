/*!
    \file  readme.txt
    \brief description of the master and slave fullduplex communication use dma demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows SPI0 and SPI1 fullduplex 
communication use dma mode.After the communicate is complete,if receive data equal to 
send data,led1 and led2 turn on, if not led1 and led2 turn off.

  Connect SPI0 SCK  PIN(PA5) TO SPI1 SCK  PIN(PB13)
  Connect SPI0 MISO PIN(PA6) TO SPI1 MISO PIN(PB14)
  Connect SPI0 MOSI PIN(PA7) TO SPI1 MOSI PIN(PB15)