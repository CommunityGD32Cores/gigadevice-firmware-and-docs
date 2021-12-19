/*!
    \file    readme.txt
    \brief   description of the USB Custom HID device demo

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

  This demo is based on the GD32W515P-EVAL-V1.0 board,it provides a description 
of how to use the USB-Device peripheral.

  The GD32 device is enumerated as an USB Custom HID device, that uses the 
native PC Host HID driver.

  To test the demo, you need to:
    - Download the "Custom_HID Client" PC applet
    - Start the "Custom_HID Client" PC applet and connect GD32 USB to PC
    - The device should be detected and shown in the PC applet 
    - Make sure that following report ID are configured: LED1 ID (0x11),
      LED2 ID(0x12), LED3 ID(0x13), BUTTON1_ID(0x15).
    - Select Leds in the applet to switch on/off on the EVAL board 
    - Press the TAMPER_WAKEUP on the EVAL board to switch on
      /off the button1 status in the PC applet
      
  If IRC8M is used, USE_IRC48M must be defined. 