/*!
    \file  readme.txt
    \brief description of the master and slave fullduplex communication with ti mode 
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows SPI0 and SPI1 fullduplex communication with ti mode. After the 
communicate is complete, if receive data equal to send data,led1 and led2 turn on,
if not led1 and led2 turn off.

  Connect SPI0 NSS  PIN(PA4) to SPI1 NSS  PIN(PB12)
  Connect SPI0 SCK  PIN(PA5) to SPI1 SCK  PIN(PB13)
  Connect SPI0 MISO PIN(PA6) to SPI1 MISO PIN(PB14)
  Connect SPI0 MOSI PIN(PA7) to SPI1 MOSI PIN(PB15)