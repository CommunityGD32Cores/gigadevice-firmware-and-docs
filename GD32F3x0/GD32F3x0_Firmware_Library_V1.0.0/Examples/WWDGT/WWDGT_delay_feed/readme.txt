/*!
    \file  readme.txt
    \brief description of the WWDGT_delay_feed example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows the WWDGT
with different counter value and window value make the different WWDGT timeout. As
a result, LED1 and LED2 display a different phenomenon. 
  
  In this demo, the WWDGT counter value is set to 127, the refresh window is set to 
80. In this case the WWDGT timeout is set to 38.8ms, the value in the delay function 
is set to 30ms, the WWDGT counter can reloaded normally,the LED1 is turn off, LED2 
is blinking fast.
  
  If the value in the delay function is not set properly, the WWDGT counter is not 
reloaded. As a result, the system reset and the LED1 and LED2 are remain illuminated.
  
  In this example the system clock is set to 108 MHz.
