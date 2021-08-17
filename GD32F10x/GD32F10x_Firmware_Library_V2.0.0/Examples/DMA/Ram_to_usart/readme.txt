/*!
    \file  readme.txt
    \brief description of DMA ram to usart
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This example is based on the GD32107C-EVAL board, it provides a description 
of how to use DMA channel3 to transfer data from RAM memory to USART transmit data 
register.The start of transfer is triggered by software. At the end of the transfer, a 
transfer complete interrupt is generated since it is enabled. If the DMA transfer
operation is finished correctly, data stored array welcome[] will be transfered 
to a serial port tool by USART and LED1 light up. 
  JP5 and JP6 must be jumpped to USART.