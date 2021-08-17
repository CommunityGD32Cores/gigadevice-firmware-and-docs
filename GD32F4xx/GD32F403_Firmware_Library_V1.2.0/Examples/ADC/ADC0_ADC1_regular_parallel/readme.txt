/*!
    \file  readme.txt
    \brief description of the ADC0_ADC1_regular_parallel example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo shows how to use ADC0 and ADC1 regular_parallel convert 
function. PA3 and PA4 are configured in AIN mode. TIMER0_CH0 is the 
trigger source of ADC0. ADC1 external trigger chooses none. When the
rising edge of TIMER0_CH0 coming, ADC0 and ADC1 regular channels are
triggered at the same time. The values of ADC0 and ADC1 are transmit
to array adc_value[] by DMA.
  
  We can watch array adc_value[] in debug mode. 
  
 