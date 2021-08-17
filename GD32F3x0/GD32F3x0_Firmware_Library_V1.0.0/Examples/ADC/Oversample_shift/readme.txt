/*!
    \file  readme.txt
    \brief description of the ADC oversample shift example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to use
the function of oversample and shift. In this demo, 16 times ratio of
oversample and 4 bits shift are configured. PC1(channel11) is chosen as
analog input pin. The ADC conversion begins by software, the converted 
data is printed by USART.

  The analog input pin should configured to analog mode.
  Jump the JP13 to USART.