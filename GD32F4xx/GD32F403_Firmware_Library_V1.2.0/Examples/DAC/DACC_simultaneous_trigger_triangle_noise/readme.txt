/*!
    \file  readme.txt
    \brief description of DACC_simultaneous_trigger_triangle_noise example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This example shows how to use DAC concurrent mode simultaneous trigger to generate triangle 
noise wave with same amplitude. TIMER5 is chosen to trigger DAC0 and DAC1. The DAC0 output pin 
is configured PA4 and DAC1 output pin is configured PA5. The triangle noise wave can be observed 
through the scilloscope.
