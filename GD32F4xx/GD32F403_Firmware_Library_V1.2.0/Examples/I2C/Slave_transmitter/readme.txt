/*!
    \file  readme.txt
    \brief description of the slave transmitter
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/


    This shows the I2C programming mode in slave transmitting mode.In this demo,
I2C0 is operated as slave transmitter,and the SCL line and SDA line of I2C0 
interface are controled by the I/O pin PB6 and PB7 respectively.
    This demo shows the sending data process of the slave. And it will send the 
  data in the i2c_transmitter array through the I2C0.
    The demo doesn't perform the data transfer actually, which is due to no specific 
  master.In the specific application, we need config the master to start a data 
  transmission.In addition,the master and the slave may need to be connected by 
  the jumper if necessary.When the macro I2C_10BIT_ADDRESS is 1, I2C communicate in 
  10 bit addressing mode, otherwise, I2C communicate in 7 bit addressing mode.
