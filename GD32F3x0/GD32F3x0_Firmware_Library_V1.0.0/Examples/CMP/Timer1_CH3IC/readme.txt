/*!
    \file  readme.txt
    \brief description of the comparator output timer input capture demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

    This demo is based on the GD32350R-EVAL board, it shows how to configure the
  comparator trigger timer 1 channel 3 input capture event, which is configured 
  to generate an interrupt on both rising and falling edge of the output signal.
  In this demo, input 3.3V to CMP0_IP. After system start-up, enable comparator 
  and trigger interrupt,then LED3 is on.  

    Connect PA1 to +3V3.   