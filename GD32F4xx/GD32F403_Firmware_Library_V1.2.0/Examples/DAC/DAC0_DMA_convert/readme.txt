/*!
    \file  readme.txt
    \brief description of DAC0_DMA_convert example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This example shows how to use DAC0 to generate a staircase-shaped signal. TIMER5 is 
chosen to trigger DAC0 and DMA is configured to transfer data from memory to DAC0_R8DH. 
The DAC0 output pin is configured PA4. The signal can be observed through the oscilloscope.
