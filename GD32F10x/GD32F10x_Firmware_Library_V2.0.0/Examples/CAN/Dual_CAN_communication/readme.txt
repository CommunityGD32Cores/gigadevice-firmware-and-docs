/*!
    \file  readme.txt
    \brief description of the dual can communication demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to configure the 
CAN0 and CAN1 peripherals to send and receive CAN frames in normal mode. Pressing 
Wakeup key or Tamper key:

  When Tamper key is pressed, CAN0 sends a message to CAN1 and print it.when CAN1 
receives correctly this message,the receive data will be printedand LED4 toggles 
one time. 

  When Wakeup key is pressed, CAN1 sends a message to CAN0 and print it. When CAN0 
receives correctly this message,the receive data will be printed and LED3 toggles 
one time.

  User can select one from the preconfigured CAN baud rates from the private 
defines in main.c. These baudrates is correct only when the system clock frequency 
is 108M.

  P2,P3 and P4 must be fitted.
  Connect JP14 CAN_L to JP15 CAN_L
  Connect JP14 CAN_H to JP15 CAN_H
