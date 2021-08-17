/*!
    \file  readme.txt
    \brief description of the USB MSC device(Udisk) demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board,it provides a description of 
how to use the USB device application based on the Mass Storage Class (MSC).

  The GD32 MCU is enumerated as a MSC device using the native PC Host MSC driver
to which the GD32F350-EVAL board is connected.

  It illustrates an implementation of the MSC class which uses the bulk transfer
while the internal sram is used as storage media.

  This example supports the BOT (bulk only transfer) protocol and all needed SCSI
(small computer system interface) commands, and is compatible with Windows platform.

  After running the example, the user just has to plug the USB cable into a PC host
and the device is automatically detected. One new removable drives appear in the
system window and write/read/format operations can be performed as with any other
removable drive.

  To select the appropriate USB Core to work with, user must add the following macro 
defines within the compiler preprocessor (already done in the preconfigured projects 
provided with this application):
  - "USE_USB_FS" when using USB Full Speed (FS) Core

  In order to make the program work, you must do the following :
    - Open your preferred toolchain 

    - Rebuild all files and load your image into target memory

    - Run the application
