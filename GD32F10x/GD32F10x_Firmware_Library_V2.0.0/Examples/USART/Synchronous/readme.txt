/*!
    \file  readme.txt
    \brief USART synchronous
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it provides a basic 
communication between USART0 (Synchronous mode) and SPI0. First, USART0
transmit a series of data to SPI0, then SPI0 transmit a series of data 
to USART0. Compare the transmitted data and the received data. If the 
data transmitted from USART0 and received by SPI0 are the same, LED1 and
LED3 will on, otherwise off. If the data transmitted from SPI0 and 
received by USART0 are the same, LED2 and LED4 will on, otherwise off.
 
  connect SPI0 SCK  pin(PA5) TO USART0_CK  pin(PA8)
  connect SPI0 MISO pin(PA6) TO USART0_RX  pin(PA10)
  connect SPI0 MOSI pin(PA7) TO USART0_TX  pin(PA9)

