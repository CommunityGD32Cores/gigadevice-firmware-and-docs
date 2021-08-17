/*!
    \file    readme.txt
    \brief   description of the USB HID BCD

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

  This demo is based on the GD32E507Z-EVAL-V1.0 board, it provides a description of 
how to use the USB_Device peripheral.

  The GD32 device is enumerated as an USB keyboard, that uses the native PC Host
HID driver. The USB keyboard with Battery Charger Detection (BCD).

  The GD32E50x device supports charging port detection (BCD) described in Battery Charging 
Specification Revision 1.2.

   The GD32 include the capability to detect various USB battery chargers and can detect 
a range of USB battery chargers including a standard downstream port (SDP), a dedicated 
charging port (DCP), a charging downstream port (CDP), and a personal system 2 port (PS2).

  For more information on USB battery charger detection, please see the USB BDC 1.2.

  In this example, we use GD32 board LEDs to indicate the following connection status events:
    - LED1 for detection of SDP (host port without charging capability)
    - LED2 for detection of DCP (host port with charging capability)
    - LED3 for detection of CDP (Charger with short circuit between D+ and D-)
    - LED4 for detection of PS2

  For keyboard specific operation method, please refer to hid_keboard demo.

  Note: In BCD test, power on the development board first, and then insert usb line to PD port.
