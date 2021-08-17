/*!
    \file  readme.txt
    \brief Description of the USB CDC_ACM demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, demo for GD32F30x
*/

  The target of this example is to read data from and write data to USB devices using 
the CDC protocol. 
  It makes the USB device look like a serial port (NO serial cable connectors: You 
can see the data transferred to and from via USB instead of USB-to-USART bridge connection). 
  This example loops back the contents of a text file over usb port. To run the example, 
Type a message using the Pc's keyboard. Any data that shows in HyperTerminal is received 
from the device.
  This CDC_ACM Demo provides the firmware examples for the GD32F30x families.