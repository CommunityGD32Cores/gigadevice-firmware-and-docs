/*!
    \file  readme.txt
    \brief description of the dual can communication demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to configure the CAN0 and CAN1 peripherals to send and 
receive CAN frames in normal mode. Pressing Wakeup button or Tamper button:

  When Tamper button is pressed, CAN0 sends a message to CAN1 and print it.
when CAN1 receives correctly this message,the receive data will be printed
and LED4 toggles one time. 

  When Wakeup button is pressed, CAN1 sends a message to CAN0 and print it.
when CAN0 receives correctly this message,the receive data will be printed
and LED3 toggles one time.

  User can select one from the preconfigured CAN baud rates from the private 
defines in main.c. These baudrates is correct only when the system clock frequency 
is 168M.


  Connect JP38 CAN_L to JP40 CAN_L
  Connect JP38 CAN_H to JP40 CAN_H
