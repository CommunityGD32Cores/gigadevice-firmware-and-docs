/*!
    \file  readme.txt
    \brief description of the master transmitter
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows the I2C programming 
mode in master transmitting mode.In this demo,I2C0 is operated as the master 
transmitter, and the SCL line and SDA line of I2C0 interface are controled by 
the I/O pin PB6 and PB7 respectively.
  This demo shows the sending process of the master. And it will send the data 
in the i2c_transmitter array through I2C0.
  The demo doesn't perform the data transfer actually, which is due to no specific 
slave. In the specific application, we must send the correct slave address, and the 
master and the slave may need to be connected by the jumper if necessary.

