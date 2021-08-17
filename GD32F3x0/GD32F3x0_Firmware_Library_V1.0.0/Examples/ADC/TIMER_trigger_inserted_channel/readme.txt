/*!
    \file  readme.txt
    \brief description of TIMER trigger inserted channel of ADC demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to convert 
ADC inserted group channels continuously using TIMER1 external trigger. 
The inserted group length is 4, the scan mode is set, every compare event 
will trigger ADC to convert all the channels in the inserted group.
