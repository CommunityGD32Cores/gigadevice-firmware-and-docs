/*!
    \file  readme.txt
    \brief description of the backup data demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to store user data in the backup data registers.
  As the backup domain still powered by VBAT when VDD is switched off, its contents
are not lost if a battery is connected to VBAT pin. 
  JP9 must be connected to 3.3V before the board is powered up, and LED2, LED3 and 
LED4 are on. After an external reset, LED2 and LED3 are off, LED4 is on. Change JP9 
connected to external battery, then execute a power-down and power-up operation, 
LED1, LED3 and LED4 are on.

  BT1 should have a 3.3V battery, JP9 can change the VBAT source.  
