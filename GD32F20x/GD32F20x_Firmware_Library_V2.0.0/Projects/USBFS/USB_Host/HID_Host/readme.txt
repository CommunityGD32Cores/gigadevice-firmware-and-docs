/*!
    \file  readme.txt
    \brief description of the usb host mode to control hid device
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, and it provides a description of 
how to use the USBFS host peripheral on the GD32F20x devices.
  
  When an USB Device is attached to the Host port, the device is enumerated and checked
whether it can support HID device, if the attached device is HID one, when the user
press the user key, the mouse or the keyboard application is launched.

  If a mouse has been attached, moving the mouse will print the position of the mouse and 
the state of botton through the serial port.

  If a keyboard has been attached, pressing the keyboard will print the state of the button
through the serial port.

Note: In the USB Host HID class, two layouts are defined in the usbh_hid_keybd.h file
      and could be used (Azerty and Querty)
        //#define QWERTY_KEYBOARD
        #define AZERTY_KEYBOARD
      The User can eventually add his own layout by editing the HID_KEYBRD_Key array
      in the usbh_hid_keybd.c file.