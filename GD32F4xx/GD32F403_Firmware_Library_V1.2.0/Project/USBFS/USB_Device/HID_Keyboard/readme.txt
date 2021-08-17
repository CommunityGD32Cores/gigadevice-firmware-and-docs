/*!
    \file  readme.txt
    \brief description of the USB HID device(USB-Keyboard) demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, demo for GD32F403
*/

  This demo is based on the GD32F403Z-EVAL board, it provides a description of 
how to use the USB_FS device application based on the Human Interface Device (HID).

  The Full Speed (FS) USB module uses a 48MHz clock, which is generated 
from an integrated PLL.

  The GD32 device is enumerated as an USB keyboard, that uses the native PC Host
HID driver to which the GD32F403Z-EVAL board is connected.

  The USB keyboard use three key(wakeup key, tamper key and user1 key) to print 3 
characters('b', 'a' and 'c').

  This demo supports remote wakeup (which is the ability of a USB device to 
bring a suspended bus back to the active condition), and the wakeup key is 
used as the remote wakeup source.

  In order to test USB remote wakeup function, you can do as follows:
    - Manually switch PC to standby mode

    - Wait for PC to fully enter the standby mode

    - Push the wakeup key

    - If PC is ON, remote wakeup is OK, else failed

  In order to make the program work, you must do the following:
    - Open your preferred toolchain 

    - Rebuild all files and load your image into target memory

    - Run the application
