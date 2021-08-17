/*!
    \file  readme.txt
    \brief description of the comparator pwm signal control demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to control PWM 
  output by using comparator output

  - CMP channel0 is configured as following:
  - Inverting input is internally connected to VREFINT = 1.22V
  - Non Inverting input is connected to PC1
  - Output is internally connected to TIMER1 OCREFCLR (output compare reference clear)

  - While PC1/PC0 is lower than VREFINT (1.22V), PWM signal is displayed on PB3 
  - While PC1/PC0 is higher than VREFINT, PB3 is displayed in low level.

  Connect PA1 to PC1.