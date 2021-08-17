/*!
    \file  readme.txt
    \brief description of the USB HID device(USB-Keyboard) demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-05-31, V1.0.0, firmware for GD32F3x0
*/

  This demo is based on the GD32350R-EVAL board,it presents the implementation 
of a device firmware upgrade (DFU) capability in the GD32 USB device.

  It follows the DFU class specification defined by the USB Implementers Forum for 
reprogramming an application through USBFS.

  The DFU principle is particularly well suited to USBFS applications that 
need to be reprogrammed in the field.

  To test the demo, you need a configuration hex image or bin image . The hex image 
and the bin image should set application address at 0x8004000. You can refer to 
"../Utilities/Binary/DFU_Images" folder and use the hex images and bin images in it.

  You need install the corresponding GD DFU Driver with your PC operation system. 

  Once the configuration *.hex image is generated, it can be downloaded into 
internal flash memory using the GD tool "GD MCU Dfu Tool" available for download 
from www.gd32mcu.21ic.com.

  The supported memory for this example is the internal flash memory, you can also
add a new memory interface if you have extral memory.

After each device reset, hold down the TAMPER key on the GD32F350-EVAL board.
