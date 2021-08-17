/*!
    \file  readme.txt
    \brief description of the ADC regular channel with DMA
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board, it shows how to use the ADC 
to convert analog signal to digital data through DMA. The ADC is configured
in continuous mode, PC1 is chosen as analog input pin.
  
  As the ADC convertion begins by software, the converted data from ADC_RDATA 
register to SRAM begins continuously. Users can change the VR1 on the GD32350R-EVAL
board, measure TP1 pad board, and check if its value matches the converted 
data through the watch window.

  The analog input pin should configured to AIN mode.
