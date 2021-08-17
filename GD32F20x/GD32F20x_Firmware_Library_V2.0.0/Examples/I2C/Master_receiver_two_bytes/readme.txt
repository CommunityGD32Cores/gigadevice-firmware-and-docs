/*!
    \file  readme.txt
    \brief description of the master receiver two bytes
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows the I2C programming 
mode in master receiving mode which the master only want to receive two bytes of
data. In this demo,I2C1 is operated as master receiver,and the SCL line and SDA 
line of I2C1 interface are controled by the I/O pin PB6 and PB7 respectively.

  This demo shows the receiving two bytes process of the master. And it will store 
the received data in the i2c_receiver array.

  This demo doesn't perform the data transfer actually, which is due to no 
specific slave.In the specific application, we must send the correct slave 
address, and the master and the slave may need to be connected by the jumper 
if necessary.
