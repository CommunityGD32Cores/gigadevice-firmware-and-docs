/*!
    \file  readme.txt
    \brief description of the Write_Protection example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.1, firmware for GD32F403
*/

  This demo shows how to enable and disable the write protection for the embedded flash.
  To enable the write Protection, uncomment the line "#define WRITE_PROTECTION_ENABLE"
in main.c file. 
  To disable the Write Protection, uncomment the line "#define WRITE_PROTECTION_DISABLE"
in main.c file.
  If the specified pages are not write protected, an erase and a write operation are
performed.
