/*!
    \file  readme.txt
    \brief description of DAC0_DMA_convert example
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it shows how to use DAC0 to generate
a staircase-shaped signal. TIMER5 is chosen to trigger DAC0 and DMA is configured to transfer
data from memory to DAC0_R8DH. The DAC0 output pin is configured PA4. The signal can be 
observed through the oscilloscope.
