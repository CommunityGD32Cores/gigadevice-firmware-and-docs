/*!
    \file  readme.txt
    \brief Description of the USB CCID demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=5)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=5)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=5)
*/

  This demo is based on the GD32150R-EVAL board, it shows how to read data from
and write data to USB devices using the CCID protocol.This demo should combine
upper computer and CCID device, the present smart card is china mobile card. 

  Consider of lacking smart card driver, the demo realize partial function of smart
card. Through upper computer, write data to smart card. likewise, smard card could
send the feedback to upper computer through CCID device. 