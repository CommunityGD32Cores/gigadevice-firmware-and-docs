/*!
    \file  readme.txt
    \brief description of the slave receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows the I2C programming 
mode in slave receiving mode. In this demo, I2C0 is operated as slave receiver, 
and the SCL line and SDA line of I2C0 interface are controled by the I/O pin 
PB6 and PB7 respectively.

  This demo shows the receving process of the slave. And it will store the 
received data in the i2c_receiver array.

  The demo doesn't perform the data transfer actually, which is due to no specific 
master.In the specific application, we need config the master to start a data 
transmission.In addition,the master and the slave may need to be connected by 
the jumper if necessary.

  P4 must be fitted.
