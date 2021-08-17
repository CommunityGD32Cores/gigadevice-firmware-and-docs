/*!
    \file  readme.txt
    \brief description of the TIMER0 DMA burst demo for gd32f30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to use DMA with 
TIMER0 update request to transfer data from memory to TIMER0 capture compare 
register 0~3.
 
  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 119
so the TIMER0 counter clock used is 1MHz.

  The objective is to configure TIMER0 channel 0~3(PA8~PA11) to generate PWM signal.
capture compare register 0~3 are to be updated twice per circle.On the first update 
DMA request, data1 is transferred to CH0CV, data2 is transferred to CH1CV, data3 is 
transferred to CH2CV,data4 is transferred to CH3CV and duty cycle(10%,20%,30%,40%). 
On the second update DMA request, data5 is transferred to CH0CV, data6 is transferred 
to CH1CV, data7 is transferred to CH2CV,data8 is transferred to CH3CV and duty cycle
(50%,60%,70%,80%).
