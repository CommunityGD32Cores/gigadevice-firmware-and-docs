/*!
    \file    readme.txt
    \brief   description of the USB host mode to control CDC device

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
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

  This demo is based on the GD32E507Z-EVAL-V1.0 board, it provides a description of how to use
the USBHS peripheral.

  This demo is USB host application based on the Communication Class (CDC). Insert the OTG cable
to the USB_FS port. Then download it to the EVAL board and run.

  This is a typical application on how to use USBHS peripheral to operate with an USB CDC
device application based on the two CDC transfer directions with a dynamic serial configuration:

 - Transmission:
   The user can select in the Host board, using the menu, a file among the ones available on the 
   SRAM and send it to the Device board.

 - Reception:
   The data are transferred by the device board to the Host board and displayed on its LCD screen.

  When the application is started, the connected USB CDC device is detected in CDC mode and gets
initialized. The STM32 MCU behaves as a CDC Host, it enumerates the device and extracts VID, PID,
manufacturer name, Serial no and product name information and displays it on the LCD screen.

  A menu is displayed and the user can select any operation from the menu using the CET key, A key and C key:
    - "Send Data" operation starts the Data Transmission.

    - "Receive Data" operation starts the Data Reception.

    - "Configuration" operation defines the desired Host CDC configuration (Baudrate, Parity, DataBit and StopBit)
       The baudrate comes with a default value of 115.2 kbps (BAUDRATE = 115200).

  Through pressing the A key and C key to select menu, next pressing the CET key to confirm selection.
