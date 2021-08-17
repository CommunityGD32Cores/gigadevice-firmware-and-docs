/*!
    \file  readme.txt
    \brief description of the WWDGT_delay_feed example
*/

/*
    Copyright (C) 2017 GigaDevice
    
    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/
  This demo is based on the GD32207i-EVAL board, it shows the WWDGT with different
counter value and window value make the different WWDGT timeout. As a result, LED1 
and LED2 display a different phenomenon.

  In this demo, the WWDGT counter value is set to 127, the refresh window is set to 
80. In this case the WWDGT timeout is set to 34.9ms(the timeout may varies due to 
APB1 frequency dispersion), the value in the delay function is set to 26ms, the WWDGT 
counter can reloaded normally,the LED1 is turn off, LED2 is blinking fast.
  
  If the value in the delay function is not set properly, the WWDGT counter is not 
reloaded. As a result, the system reset and only the LED1 is remain illuminated.
  
  In this example the system clock is set to 120 MHz , and the APB1 is set to 60MHz.
