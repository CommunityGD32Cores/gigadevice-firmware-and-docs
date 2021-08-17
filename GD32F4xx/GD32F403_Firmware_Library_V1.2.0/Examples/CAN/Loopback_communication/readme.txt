/*!
    \file  readme.txt
    \brief description of the can loopback communication demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to set a communication with the CAN in loopback mode.

  The example first performs a transmission and a reception of a standard data frame by 
polling at 125 Kbps.If the received frame is successful, the LED1 and LED2 are on. 
otherwise,the LED1 and LED2 are off. Then, an extended data frame is transmitted at 125 Kbps. 
Reception is done in the interrupt handler when the message becomes pending in the FIFO1. If 
the received frame is successful, the LED3 and LED4 are on. otherwise,the LED3 and LED4 are off.
    
  User can select CAN0 or CAN1 cell using the private defines in main.c.
