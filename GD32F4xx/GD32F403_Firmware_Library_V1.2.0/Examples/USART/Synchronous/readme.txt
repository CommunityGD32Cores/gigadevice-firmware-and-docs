/*!
    \file  readme.txt
    \brief USART synchronous
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows a basic communication between USART0 (Synchronous mode) 
and SPI0. First, USART0 transmits a series of data to SPI0, then SPI0 
transmits a series of data to USART0. Compare the transmitted data and 
the received data. If the data transmitted from USART0 and received by 
SPI0 is the same, LED1 andLED3 will be on, otherwise off. If the data 
transmitted from SPI0 and received by USART0 is the same, LED2 and LED4 
will be on, otherwise off.

  connect SPI0 SCK  pin(PA5) TO USART0_CK  pin(PA8)
  connect SPI0 MISO pin(PA6) TO USART0_RX  pin(PA10)
  connect SPI0 MOSI pin(PA7) TO USART0_TX  pin(PA9)

