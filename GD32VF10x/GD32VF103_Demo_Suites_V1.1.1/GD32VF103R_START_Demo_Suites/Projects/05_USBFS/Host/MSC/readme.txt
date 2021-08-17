/*!
    \file  readme.txt
    \brief Description of the USB host mode to control MSC device

    \version 2019-06-05, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

  This demo is based on the GD32VF103R-START-V1.0 board, and it provides a description of how 
to use the USBFS host peripheral on the GD32VF103V devices.

  Insert the OTG cable to the USB_FS port. Then download it to the start board and run.

  If an Udisk has been attached, the LED4 will be light on, indicating that the U disk is 
successfully connected. And the LED 2 flickers slowly, indicating that the U disk has been 
successfully enumerated. 

  First pressing the User key, LED2 will flickers quickly, indicating that the host has 
identified the U disk. 

  Then pressing the User key again, LED2 will flickers slowly, indicating that the contents 
of the U disk have been read correctly. 

  The third press of the User button will write files to the U disk and the user will see 
both the LED 1 and the LED 2 light up, indicating the end of the MSC host example. 

 Finally, pull out the U disk and you can see that the LED 4 is on. 
