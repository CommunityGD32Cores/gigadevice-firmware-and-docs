/*!
    \file  readme.txt
    \brief description of the Erase_Program example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

  This demo shows how to erase_program flash memory.
  Before an erase operation, fmc_unlock function is used firstly to unlock 
the flash memory program/erase controller. 
  After the erase operation, a comparison between flash memory and 0xFFFFFFFF(Reset value) 
is done to check that the flash memory has been correctly erased.
  Once the erase operation is finished correctly, the programming operation will be
performed by using the fmc_word_program function. A comparison between the flash memory 
and the source data is done to check that all data have been correctly programed.
