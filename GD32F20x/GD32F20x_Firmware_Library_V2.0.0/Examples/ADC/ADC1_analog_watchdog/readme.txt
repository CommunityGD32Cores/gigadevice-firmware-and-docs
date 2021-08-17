/*!
    \file  readme.txt
    \brief description of the ADC1 analog watchdog example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to
use the ADC1 analog watchdog to guard continuously an ADC channel.
The ADC1 is configured in continuous mode, the ADC1 clock is configured
to 20MHz. PC3(CHANNEL13) is chosen as analog input pin.

  Change the VR1 on the GD32F207i-EVAL board, measure TP2 pad on the 
GD32F207i-EVAL board, when the CHANNEL13 converted value is over the 
programmed analog watchdog high threshold(value 0x0A00) or below 
the analog watchdog low threshold(value 0x0400), an WDE interrupt 
will occur, and LED2 will be on. When the CHANNEL13 converted value is
in safe range(among 0x0400 and 0x0A00), the LED2 will be off.

  The analog input pin should be configured to AIN mode. 
