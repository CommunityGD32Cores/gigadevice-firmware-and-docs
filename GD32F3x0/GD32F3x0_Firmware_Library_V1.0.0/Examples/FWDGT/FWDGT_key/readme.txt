/*!
    \file  readme.txt
    \brief description of the FWDGT_key example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows how to 
reload the FWDGT counter at regulate period using the EXTI interrupt. The FWDGT 
timeout is set to 1s (the timeout may varies due to IRC40K frequency dispersion).
 
  An EXTI is connected to a specific GPIO pin and configured to generate an interrupt
on its falling edge: when the EXTI Line interrupt is triggered (by pressing the Tamp Key 
on the board), the corresponding interrupt is served. In the ISR, FWDGT counter is reloaded). 
As a result, when the FWDGT counter is reloaded, which prevents any FWDGT reset, LED2 remain
illuminated.   
  
  If the EXTI Line interrupt does not occur, the FWDGT counter is not reloaded before
the FWDGT counter reaches 00h, and the FWDGT reset. If the FWDGT reset is generated, LED2 
is turned off with the system reset.
  
  In this example the system clock is set to 108 MHz.
