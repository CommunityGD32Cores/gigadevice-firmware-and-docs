/*!
    \file  readme.txt
    \brief description of ADC channel of temperature and vref
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to 
use the ADC to convert analog signal to digital data. The ADC is configured 
in dependent mode, inner channel16(temperature sensor channel) and channel17
(VREF channel) are chosen as analog input pin.

  As the ADC convertion begins by software, the converted data in the 
ADC_IDATAx register, where the x is 0 to 1.
 