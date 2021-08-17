/*!
    \file  readme.txt
    \brief description of the USB Audio demo.
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

  The Audio device example allows device to communicate with host (PC) as USB headphone
using isochronous pipe for audio data transfer along with some control commands (i.e.
Mute). 
  It follows the "Universal Serial Bus Device Class Definition for Audio Devices
Release 1.0 March 18, 1998" defined by the USB Implementers Forum for reprogramming
an application through USB-FS-Device. 
  Following this specification, it is possible to manage only Full Speed USB mode 
(High Speed is not supported). 

  This class is natively supported by most Operating Systems (no need for specific
driver setup).

  This example uses the I2S interface to stream audio data from USB Host to the audio
codec implemented on the evaluation board. Then audio stream is output to the Headphone.

  For the GD32F150 board, it possible to use one of the two quartz belwo:
  - 14.7456MHz which provides best audio quality
  - Standard 25MHz which provides lesser quality.

  The device supports one audio frequency (the host driver manages the sampling rate
conversion from original audio file sampling rate to the sampling rate supported
by the device). It is possible to configure this audio frequency by modifying the
usbd_conf.h file (define USBD_AUDIO_FREQ). It is advised to set high frequencies
to guarantee a high audio quality.  
  It is also possible to modify the default volume through define DEFAULT_VOLUME in file
usbd_conf.h.

note: The audio frequencies leading to non integer number of data (44.1KHz, 22.05KHz, 
       11.025KHz...) will not allow an optimum audio quality since one data will be lost
       every two/more frames.
