/*!
    \file  readme.txt
    \brief description of the master receiver one byte
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

  This demo shows the I2C programming mode in master receiving mode which the 
master only want to receive one byte of data. In this demo,I2C1 is operated as 
master receiver,and the SCL line and SDA line of I2C1 interface are controled 
by the I/O pin PB6 and PB7 respectively.

  This demo shows the receiving one byte process of the master. And it will store 
the received data in the i2c_receiver array.

  This demo doesn't perform the data transfer actually, which is due to no 
specific slave.In the specific application, we must send the correct slave 
address, and the master and the slave may need to be connected by the jumper 
if necessary.
