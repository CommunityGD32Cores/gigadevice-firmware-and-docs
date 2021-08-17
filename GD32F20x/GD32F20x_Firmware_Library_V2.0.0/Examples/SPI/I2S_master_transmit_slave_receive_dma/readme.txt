/*!
    \file  readme.txt
    \brief description of I2S master send and slave receive communication use DMA demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32F207i-EVAL board, it shows I2S1 master send and I2S2 
slave receive communication use DMA mode. After the communicate is complete, if receive 
data equal to send data, led1 turn on, if not led1 turn off.

  Connect I2S1 WS PIN(PB12) to I2S2 WS PIN(PA4).
  Connect I2S1 CK PIN(PB13) to I2S2 CK PIN(PC10).
  Connect I2S1 SD PIN(PB15) to I2S2 SD PIN(PC12).