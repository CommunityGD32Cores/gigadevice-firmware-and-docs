/*!
    \file  readme.txt
    \brief description of the usb host mode to control hid device
*/

/*
    Copyright (C) 2016 GigaDevice

    2017-02-10, V1.0.0, demo for GD32F403
*/

  This demo is based on the GD32F403-EVAL board, and it provides a description of 
how to use the USBFS host peripheral on the GD32F403 devices.
  
 When an USB Device is attached to the Host port, the device is enumerated and checked
whether it can support HID device, if the attached device is HID one, when the user
press the user key, the mouse or the keyboard application is launched.

  If a mouse has been attached, moving the mouse will move the 'x' in the LCD screen and 
pressing the button will show the magenta color rectangle in the LCD screen.

  If a keyboard has been attached, pressing the keyboard will printthe the char of the button
in the LCD screen.


Note: In the USB Host HID class, two layouts are defined in the usbh_hid_keybd.h file
      and could be used (Azerty and Querty)
        //#define QWERTY_KEYBOARD
        #define AZERTY_KEYBOARD
      The User can eventually add his own layout by editing the HID_KEYBRD_Key array
      in the usbh_hid_keybd.c file.