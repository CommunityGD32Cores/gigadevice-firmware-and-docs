/*!
    \file  readme.txt
    \brief Description of the USB host mode to control MSC device
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32207i-EVAL board, and it provides a description of how 
to use the USBFS host peripheral on the GD32F20X devices.

  Insert the OTG cable to the USB_FS port. Then download it to the EVAL board and run.

  If an Udisk has been attached, the user will see the information of Udisk enumeration. 
First pressing the user key will see the Udisk information, next pressing the tamper key
will see the root content of the Udisk, then press the wakeup key will write file to the
Udisk, finally the user will see information that the msc host demo is end.
