/*!
    \file  readme.txt
    \brief description of the ADC analog watchdog example
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32F107C-EVAL board, it shows how to
use the ADC analog watchdog to guard continuously an ADC channel.
The ADC is configured in continuous mode, the ADC1 clock is configured
to 27MHz. PA3 is chosen as analog input pin.

  Change the VR1 on the GD32F107C-EVAL board, measure TP2 pad on the 
GD32F10X-EVAL board, when the channe3 converted value is over the 
programmed analog watchdog high threshold (value 0x0A00) or below 
the analog watchdog low threshold(value 0x0400), an WDE interrupt 
will occur, and LED2 will be on. When the channe3 converted value is
in safe range(among 0x0400 and 0x0A00), the LED2 will be off.

  The analog input pin should be configured to AIN mode. 
