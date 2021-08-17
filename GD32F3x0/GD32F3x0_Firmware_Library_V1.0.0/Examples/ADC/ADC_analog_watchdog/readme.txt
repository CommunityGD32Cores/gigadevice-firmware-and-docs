/*!
    \file  readme.txt
    \brief description of the ADC analog watchdog demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to
use the ADC analog watchdog to guard continuously an ADC channel.
The ADC is configured in continuous mode, PC1 is chosen as analog
input pin.

  Change the VR1 on the GD32350R-EVAL board, when the channel11 
converted value is over the programmed analog watchdog high threshold
(value 0x0A00) or below the analog watchdog low threshold(value 0x0400),
an WDE interrupt will occur, and LED2 will turn on. When the channel11
converted value is in safe range(among 0x0400 and 0x0A00), the LED2 will
be off.

  The analog input pin should be configured to AIN mode.
