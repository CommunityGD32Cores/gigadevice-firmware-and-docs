/*!
    \file  readme.txt
    \brief description of the can network communication demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to configure the CAN 
peripheral to send and receive CAN frames in normal mode.The frames are sent and print
transmit data by pressing Tamper Key. When the frames are received, the receive data 
will be printed and the LED1 will toggle.
 
  This example is tested with at least two GD32107C-EVAL boards. The same program example 
is loaded in all boards and connect CAN_L and CAN_H pin of CAN0 or CAN1 to bus for sending 
and receiving frames.

  User can select CAN0 or CAN1 cell using the private defines in main.c.


