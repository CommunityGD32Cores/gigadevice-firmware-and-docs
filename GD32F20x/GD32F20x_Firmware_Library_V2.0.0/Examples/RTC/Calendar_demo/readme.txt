/*!
    \file  readme.txt
    \brief description of calendar demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

    This demo is based on the GD32F207i-EVAL board, it explains how to configure the
  RTC module. In this demo, RTC peripheral is configured to display calendar.
    Select LXTAL as RTC clock source. And the HyperTerminal should be connected to the 
  evaluation board via COM1.
    After start-up,if the BKP data register is written to a key value. If the value 
  is incorrect, the program will ask to set the time on the HyperTerminal. If the
  key value is correct, by the HyperTerminal set time,the calendar will be displayed
  on the HyperTerminal. 
    The RTC module is in the Backup Domain, and is not reset by the system reset.
 