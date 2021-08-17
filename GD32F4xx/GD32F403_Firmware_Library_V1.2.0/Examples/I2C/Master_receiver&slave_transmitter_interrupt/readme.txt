/*!
    \file  readme.txt
    \brief description of the master receiver and slave transmitter interrupt
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.2, firmware for GD32F403
*/

  This demo shows how to use the I2C interrupt programming mode in master receiving 
mode or slave transmitting mode.In this demo, I2C0 is operated as the master receiver 
and I2C1 as the slave transmitter. Moreover,the SCL line and SDA line of I2C0 interface 
are controled by the I/O pin PB6 and PB7 respectively. The SCL and SDA of I2C1 are 
controled by the pin PB10 and PB11 respectively.

  This demo will send the data of i2c_buffer_transmitter array through I2C1 interface 
to the I2C0, and it will store the data received by I2C0 in the i2c_buffer_receiver
array. If transfer is sucessfully completed, LED1 and LED2 are on. 

  We shoud use the jumper to connect the PB6 and PB10. The PB7 and PB11 are 
connected as well.
