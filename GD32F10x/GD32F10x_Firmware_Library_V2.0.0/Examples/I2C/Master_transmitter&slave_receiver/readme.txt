/*!
    \file  readme.txt
    \brief description of the master transmitter and slave receiver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows the I2C programming mode in master 
transmitter mode and slave receiving mode. In this demo,I2C0 is operated as master transmitter
,I2C1 is operated as slave receiver and the SCL line and SDA line of I2C interface are controled 
by the I/O pin PB10 and PB11(or PB6 and PB7) respectively.

  This demo shows the sending data process of the master and the receiving data process of the slave.
If transfer is sucessfully completed, LED1 and LED2 is light. 

  We shoud use the jumper to connect the PB6 and PB10. The PB7 and PB11 are connected as well.


