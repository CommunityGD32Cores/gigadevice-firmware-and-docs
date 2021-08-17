/*!
    \file  readme.txt
    \brief description of tamper demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, firmware for GD32F10x
    2017-06-20, V2.0.0, firmware for GD32F10x
*/

  This demo is based on the GD32107C-EVAL board, it shows how to write/read data 
to/from backup data registers and demonstrates the tamper detection function. In 
this demo, the TAMPER pin is configured active on low level.
  After system start-up, it writes the data to all backup data registers, then check
whether the data were correctly written. If yes, LED1 is on, otherwise LED2 is on.
When the tamper key(TAMPER pin) is pressed, the backup data registers are reset and 
the tamper interrupt is generated. In the corresponding ISR, it checks whether the 
backup data registers are cleared or not. If yes, LED3 is on, otherwise LED4 is on.
