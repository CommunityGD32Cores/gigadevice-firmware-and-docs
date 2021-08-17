/*!
    \file  readme.txt
    \brief description of the FWDGT_key example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

  This demo shows how to reload the FWDGT counter at regulate period using
the EXTI interrupt. The FWDGT timeout is set to 1.6s (the timeout may varies
due to IRC40K frequency dispersion).
 
  An EXTI is connected to a specific GPIO pin and configured to generate an interrupt
on its falling edge: when the EXTI Line interrupt is triggered (by pressing the Tamper
Key on the board), the corresponding interrupt is served. In the ISR, FWDGT 
counter is reloaded). As a result, when the FWDGT counter is reloaded, which prevents 
any FWDGT reset,  LED1 or LED2 remain illuminated.
  
  If the EXTI Line interrupt does not occur, the FWDGT counter is not reloaded before
the FWDGT counter reaches 00h, and the FWDGT reset. If the FWDGT reset is generated, 
LED1 and LED2 are turned off with the system reset. FWDGTRST flag is set by hardware,
and then LED2 is turned on.
