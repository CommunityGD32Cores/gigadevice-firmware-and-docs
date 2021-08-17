/*!
    \file  readme.txt
    \brief description of the CTC trim internal 48MHz RC oscillator with GPIO
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This example is based on the GD32F30x-EVAL board, it shows CTC is used to trim internal
48MHz RC oscillator with GPIO.

  Connect the PA8 pin with the 32kHz square wave(the amplitude is 3.3V and the offset is 
1.65V).

  When the clock trim is OK, led1 will be on.