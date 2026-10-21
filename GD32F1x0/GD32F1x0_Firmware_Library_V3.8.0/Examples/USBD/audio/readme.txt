/*!
    \file    readme.txt
    \brief   description of the USB Audio demo.

    \version 2026-01-31, V3.8.0, firmware update for GD32F1x0(x=5)

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

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

  This example is based on the GD32150R-EVAL-V1.3 board, it provides a description of 
how to use the USBD peripheral.

  The audio device example allows device to communicate with host (PC) as USB headphone (ISO OUT) and 
USB microphone (ISO IN) by using isochronous pipe for audio data transfer along with some control commands 
(i.e.Mute). Users can define macros USE_USB_AD_SPEAKER and USE_USB_AD_MICPHONE using in the project 
configuration to control whether the headphone or microphone function is enabled.

  It follows the "Universal Serial Bus Device Class Definition for Audio Devices
Release 1.0 March 18, 1998" defined by the USB Implementers Forum for reprogramming
an application through USBD.

  Following this specification, it is possible to manage only Full Speed USB mode 
(High Speed is not supported). 

  This class is natively supported by most Operating Systems (no need for specific
driver setup).

  This example uses the I2S interface to stream audio data from USB Host to the audio
codec implemented on the evaluation board. Then audio stream is output to the Headphone.

  For the GD32 board, it possible to use one of the two quartz below:
  - 14.7456MHz which provides best audio quality
  - Standard 25MHz which provides lesser quality

  The device supports one audio speaker frequency (the host driver manages the sampling rate
conversion from original audio file sampling rate to the sampling rate supported by the device). 
It is possible to configure this audio speaker frequency by modifying the usbd_conf.h file
(define USBD_AUDIO_FREQ). It is advised to set high frequencies to guarantee a high audio quality.  

  It is also possible to modify the default volume through define DEFAULT_VOLUME in file
usbd_conf.h.

  Note 1: When allocating the USB RAM buffer address (AD_RX_BUF_ADDR) for the ISO OUT endpoint, 
the reserved space should be based on SPEAKER_OUT_MAX_PACKET, not SPEAKER_OUT_PACKET, to avoid 
overlapping parts in the USB RAM allocation, which could lead to audio anomalies.

  Note 2: If the audio speaker frequency is increased, the USB RAM buffer allocation must also be modified.

  Note 3: ISO IN/OUT endpoint must enable the dual-buffer feature, which means that the TX/RX buffer table addresses 
must be configured in a double-buffered mode. (The dual-buffer feature of the ISO endpoint is enabled by default by 
the USBD peripheral hardware, even if the EP_KCTL bit of the ISO endpoint is not set to 1.)

  Note 4: To ensure transmission efficiency, the USBD_DOUBLE_BUFFER_ENABLE compilation macro is enabled by default 
and it is recommended not to disable it
