/*!
    \file  readme.txt
    \brief description of the TIMER2 PWM input capture demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to use the TIMER 
peripheral to measure the frequency and duty cycle of an external signal.
  
  The TIMxCLK frequency is set to systemcoreclock, the prescaler is 108 so the 
TIMER2 counter clock is 1MHz.
  
  The external signal is connected to TIMER2 CH0 pin (PA6). 
  The rising edge is used as active edge.
  The TIMER2 CH0CV is used to compute the frequency value.
  The TIMER2 CH1CV is used to compute the duty cycle value. 

  The available frequence of the input signal is ranging from 16Hz to 20KHz.
You can use USART to watch the frequency and duty cycle of the input signal.
The USART is configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

  JP5 and JP6 must be jumpped to USART.