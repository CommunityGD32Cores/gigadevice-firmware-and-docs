/*!
    \file  readme.txt
    \brief description of the ADC TIMER trigger injected channel
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to convert ADC 
inserted group channels continuously using TIMER1 external trigger. The inserted 
group length is 4, the scan mode is set, every compare event will trigger ADC to
convert all the channels in the inserted group.
 