/*!
    \file  readme.txt
    \brief description of DACC_independent_trigger_LFSR_noise example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32207i-EVAL board, it shows how to use DAC concurrent mode
independent trigger to generate LFSR noise wave with different configurations. TIMER5 is 
chosen to trigger DAC0. TIMER6 is chosen to trigger DAC1. The DAC0 output pin is configured
PA4 and DAC1 output pin is configured PA5. The LFSR noise wave can be observed through the 
oscilloscope.
