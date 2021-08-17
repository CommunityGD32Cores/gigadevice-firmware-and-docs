/*!
    \file  readme.txt
    \brief description of I2S master send and slave receive communication use DMA demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it shows I2S1 master send and I2S2 slave 
receive communication by using DMA mode. After the communication is complete, if data received 
is equal to data transmitted, led1 turns on, if not led1 turns off.

  Connect I2S1 WS PIN(PB12) to I2S2 WS PIN(PA4).
  Connect I2S1 CK PIN(PB13) to I2S2 CK PIN(PC10).
  Connect I2S1 SD PIN(PB15) to I2S2 SD PIN(PC12).
  
  JP18 and JP19 must be jumpped to I2S.