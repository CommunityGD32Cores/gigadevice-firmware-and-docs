/*!
    \file  readme.txt
    \brief description of the Backup_Data demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to store user data 
in the Backup data registers.
  As the Backup domain still powered by VBAT when VDD is switched off, its contents
are not lost if a battery is connected to VBAT pin. When JP0 is connected to Vmcu, 
the board is powered up, LED2, LED3 and LED4 are on. After an external reset, LED2
and LED3 are off, LED4 is on. Change JP0 connected to external battery, the board is
executed a power-down and power-up operation, LED1, LED3 and LED4 are on.
  
  The program behaves as follows:
  1. After startup the program checks if the board has been powered up. If yes, the
  values in the BKP data registers are checked:
   - if a battery is connected to the VBAT pin, the values in the BKP data registers
     are retained
   - if no battery is connected to the VBAT pin, the values in the BKP data registers
     are lost
  2. After an external reset, the BKP data registers contents are not checked.

  Four LEDs are used to show the system state as follows:
  1. LED3 on / LED1 on: a POR/PDR reset occurred and the values in the BKP data
   registers are correct
  2. LED3 on / LED2 on: a POR/PDR reset occurred and the values in the BKP data
   registers are not correct or they have not yet been programmed (if it is the
   first time the program is executed)
  3. LED3 off / LED1 off / LED2 off: no POR/PDR reset occurred
  4. LED4 on: program running
  
  BT1 should have a 3.3V battery, JP0 can change the VBAT source.  

