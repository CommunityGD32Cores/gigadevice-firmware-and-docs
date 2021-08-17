/*!
    \file  readme.txt
    \brief description of the ADC regular channel with DMA
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to
use the ADC to convert analog signal to digital data through DMA. The ADC is 
configured in continuous mode, PC3 is chosen as analog input pin.
  
  As the ADC convertion begins by software, the converted data from ADC_RDATA 
register to SRAM begins continuously. Users can change the VR1 on the GD32F207i-EVAL
board, measure TP2 pad board, and check if its value matches the converted 
data through the watch window.

  The analog input pin should configured to AIN mode.
  