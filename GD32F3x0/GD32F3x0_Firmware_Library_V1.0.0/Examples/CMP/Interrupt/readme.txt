/*!
    \file  readme.txt
    \brief description of the comparator interrupt demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to configure the
comparator trigger interrupt using an external interrupt line. In this demo,input
3.3V to PA1, EXTI line 21 is configured to generate an interrupt on rising edge 
of the output signal. After system start-up, enable comparator and trigger interrupt,
then LED2 is on. 

  Connect PA1 to +3V3.
