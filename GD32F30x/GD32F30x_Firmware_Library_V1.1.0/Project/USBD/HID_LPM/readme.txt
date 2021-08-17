/*!
    \file  readme.txt
    \brief description of the USB HID LPM Test demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board,it provides a description of 
how to test the USB LPM (Link Power Management) Feature.

  The demo is based on HID Keyboard example. All modifications versus original HID 
example are under #ifdef LPM_ENABLED which is defined in usb_conf.h file.

  When LPM packet is received from Host, GD32 USB will Acknowledge the LPM packet
and it will enter in L1 suspend mode. During USB L1 suspend mode, system will be
in STOP low power mode.

  On Host L1 resume, GD32 will wakeup from STOP and USB resumes operations.

  The demo also implements the USB L1 remotewakeup, which allows device to wakeup
host during L1 suspend. L1 remote wakeup is initiated when pressing the Tamper button.

  You can test L1 suspend/resume, by running the USBCV3.0 chapter9 for USB2.0 devices 
and select (in debug mode) test "TD9.21: LPM L1 Suspend Resume Test".
  Please note that for running USBCV3.0, you'll need a PC with a USB3.0 (xHCI) host 
controller (please refer to USBCV3.0 documentation for more informations).

  You can also find in folder .\USBCV3.0_Test, the full USBCV3.0 chapter9 test report
showing the USB trace for the LPM test.

  As it can be seen in the trace, GD32 Acknowledges the LPM packet, then after 50us 
resume, GD32 is ready to receive transactions from host, it answers a GetDescriptor
request.