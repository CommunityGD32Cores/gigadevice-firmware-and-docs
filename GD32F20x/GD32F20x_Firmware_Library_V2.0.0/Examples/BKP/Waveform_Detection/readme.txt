/*!
    \file  readme.txt
    \brief description of Waveform_Detection demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to write/read data 
to/from Backup data registers and demonstrates the Waveform detection function. In 
this demo, the first Waveform detection is enabled. 
  After system start-up, it writes the data to all Backup data registers, then check
whether the data were correctly written. If yes, LED1 is on, otherwise LED2 is on.
When using short circuit cap to connect PI8 and PC13, LED3 and LED4 are all off, this 
means that receiving and checking is right, TAMPER interrupt is not happened. When no 
short circuit cap is used to connect PI8 and PC13, LED3 is on, and LED4 is off, this 
means that receiving and checking is wrong, TAMPER interrupt is happened.

