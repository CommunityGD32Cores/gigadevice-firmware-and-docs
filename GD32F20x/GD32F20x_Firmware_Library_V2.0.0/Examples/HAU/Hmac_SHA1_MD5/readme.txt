/*!
    \file  readme.txt
    \brief description of Hmac_SHA1_MD5 example
*/

/*
    Copyright (C) 2017 GigaDevice

    2015-07-15, V1.0.0, firmware for GD32F20x
    2017-06-05, V2.0.0, firmware for GD32F20x
*/

  This demo is based on the GD32207i-EVAL board, it shows how to use the HAU
peripheral to hash data using HMAC SHA-1 and HMAC MD5 algorithms.

  The key used is long type (2088 bit data).
  The HMAC SHA-1 message digest result is a 160 bit data and the HMAC MD5 message 
digest result is a 128 bit data.
  The all result data can be displayed on a PC HyperTerminal using the USART.
  JP5 must be jumpped to USART.