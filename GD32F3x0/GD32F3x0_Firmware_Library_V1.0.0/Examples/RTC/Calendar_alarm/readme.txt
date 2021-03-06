/*!
    \file  readme.txt
    \brief description of the calendar alarm example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it explains how 
to configure the RTC module. In this demo, RTC peripheral is configured to keep
time and generate an alarm interrupt.
  
  The RTC clock source can be chose to LXTAL or IRC40K by uncomment the corresponding 
define in main file. And the HyperTerminal should be connected to the evaluation 
board via USART0/USART1. The vbat bin on the board should connect to the CR1220 battery.
  
  After start-up, the program check if the BKP data register is written to a key 
value. If the value is not correct, the program will ask to set the time and alarm
value on the HyperTerminal. If the key value is correct, the time and alarm value
will be displayed on the HyperTerminal. Also the demo shows how to get the cuurent 
subsecond value and convert it into fractional format.
  
  Then LED1 and LED2 is turned on.
  
  The RTC module is in the Backup Domain, and is not reset by the system reset or 
power reset. 
  
  When the alarm interrupt generated, the LED2 toggled. The alarm is configured to 
occur once per minute.