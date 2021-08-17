/*!
    \file  readme.txt
    \brief description of DMA ram to ram
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/
  
  This example provides a description of how to use DMA channel(1 to 4) to transfer data from 
RAM to RAM. DMA Channel(1 to 4) is configured to transfer the contents of data buffer stored in 
"source_address" to the reception buffer declared in RAM(destination_address1~destination_address4).
  
  The start of transfer is triggered by software. At the end of the transfer, a comparison between 
the source and destination buffers is done to check that all data have been correctly transferred.
If transfer correctly the corresponding LED light.If transfer do not correcly,the corresponding LED is off.
