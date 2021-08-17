/*!
    \file  readme.txt
    \brief description of the ADC analog watchdog example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to use the ADC analog watchdog to guard continuously
an ADC channel. The ADC is configured in continuous mode, the ADC1 clock is 
configured to 28MHz. PA3 is chosen as analog input pin.


  Change the VR4 on the board, measure TP7 pad on the board, when the channel3
converted value is over the programmed analog watchdog high threshold (value 0x0A00)
or below the analog watchdog low threshold(value 0x0400), an WDE interrupt will occur,
and LED2 will be on. When the channel3 converted value is in safe range(among 0x0400 
and 0x0A00), the LED2 will be off.

  The analog input pin should be configured to AIN mode. 
  
  JP20, JP30 must be fitted.