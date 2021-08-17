/*!
    \file  readme.txt
    \brief description of the ADC oversample_shift example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the function
of oversample and shift. In this demo, 16 times ratio of oversample and 4 bits shift
are configured. PC3(CHANNEL13) is chosen as analog input pin. The ADC conversion begins 
by software, the converted data is printed by USART0.

  The analog input pin should configured to analog mode.
  
  Jump the JP5 to USART.
