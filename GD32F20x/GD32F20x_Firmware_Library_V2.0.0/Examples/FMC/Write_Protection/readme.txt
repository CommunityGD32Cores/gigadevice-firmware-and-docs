/*!
    \file  readme.txt
    \brief description of the Write_Protection example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This example is based on the GD32207i-EVAL board, it provides a description of how to 
enable and disable the write protection for the embedded flash.

  Uncomment the line "#define WRITE_PROTECTION_ENABLE" and "#define FLASH_PAGE_PROGRAM"
in main.c file, download the program, an error will occur to program the flash and LED2
will on.

  Uncomment the line "#define WRITE_PROTECTION_DISABLE" and "#define FLASH_PAGE_PROGRAM"
in main.c file, download the program, the flash will be write correctly and LED1 will on.

  User can check the value of FMC_WP register to verify if the write protected operation is 
successful or not.
