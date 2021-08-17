/*!
    \file  readme.txt
    \brief Description of the USB host mode to control MSC device
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/

  This demo is based on the GD32F403-EVAL board, and it provides a description of how 
to use the USBFS host peripheral on the GD32F403 devices.
  
  
 The GD32F403 devices behave as a mass storage Host that can enumerate, show content 
and write file in the attached USB flash disk. 
  
  If an Udisk has been attached, the user will see the information of Udisk enumeration. 
First pressing the User key will see the Udisk information, next pressing the Tamper key 
will see the root content of the Udisk, then press the Wakeup key will write file to the 
Udisk, finally the user will see the information that the msc host demo is end.
