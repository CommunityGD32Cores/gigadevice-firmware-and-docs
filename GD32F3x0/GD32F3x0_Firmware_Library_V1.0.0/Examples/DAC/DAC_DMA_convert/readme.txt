/*!
    \file  readme.txt
    \brief description of DAC_DMA_convert example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This example is based on the GD32350R-EVAL board, it shows how to use DAC to generate
a staircase-shaped signal. TIMER5 is chosen to trigger DAC and DMA is configured to transfer
data from memory to DAC_R8DH. The DAC output pin is configured PA4. The signal can be 
observed through the oscilloscope.
