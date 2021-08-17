/*!
    \file  readme.txt
    \brief Description of the USB host mode to control MSC device
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, and it provides a description of how 
to use the USBFS host peripheral on the GD32F30X devices.
  
  
  Jump the JP17 to USART4 to show the print message through HyperTerminal.
Then open the HyperTerminal and connect the COM4 to the PC through the serial port line. 

  Insert the OTG cable to the USB_FS port. Then download it to the EVAL board and run.

  If an Udisk has been attached, the user will see the information of Udisk enumeration. 
First pressing the user key will see the Udisk information, next pressing the tamper key
will see the root content of the Udisk, then press the wakeup key will write file to the
Udisk, finally the user will see information that the msc host demo is end.
