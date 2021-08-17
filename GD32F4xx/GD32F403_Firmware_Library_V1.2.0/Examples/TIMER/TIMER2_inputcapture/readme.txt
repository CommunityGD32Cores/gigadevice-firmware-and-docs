/*!
    \file  readme.txt
    \brief description of the TIMER2 input capture demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demoshows TIMER peripheral to measure the frequency of an external signal.
  
  The TIMxCLK frequency is set to systemcoreclock,the prescaler is 8400 so 
the TIMER2 counter clock is 20KHz.
  
  The external signal is connected to TIMER2 CH0 pin (PA6).
  The rising edge is used as active edge.
  The TIMER2 CH0CV is used to compute the frequency value.

  The available frequence of the input signal is ranging from 0.03Hz to 2KHz.
You can use USART to watch the frequency of the input signal.The USART is
configured as follow:
  - BaudRate    = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
