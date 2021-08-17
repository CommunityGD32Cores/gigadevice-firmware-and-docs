/*!
    \file  readme.txt
    \brief description of the TIMER0 6-steps demo for GD32F30x
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

  This demo is based on the GD32F30x-EVAL board, it shows how to 
configure the TIMER0 peripheral to generate three complementary TIMER0 signals 
(for BLDC ) with dead time.

  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 119 so the 
TIMER0 counter clock used is 1MHz.
  
  Channel change sequence: 
  AB`->AC`->BC`->BA`->CA`->CB`
step1:1-0  0-1  0-0   (CH0-CH0N  CH1-CH1N  CH2-CH2N)
step2:1-0  0-0  0-1   (CH0-CH0N  CH1-CH1N  CH2-CH2N)
step3:0-0  1-0  0-1   (CH0-CH0N  CH1-CH1N  CH2-CH2N)
step4:0-1  1-0  0-0   (CH0-CH0N  CH1-CH1N  CH2-CH2N)
step5:0-1  0-0  1-0   (CH0-CH0N  CH1-CH1N  CH2-CH2N)
step6:0-0  0-1  1-0   (CH0-CH0N  CH1-CH1N  CH2-CH2N)

  Connect the TIMER0 pins to an oscilloscope to monitor the different waveforms:
  - TIMER0_CH0  pin (PA8)
  - TIMER0_CH0N pin (PB13)
  - TIMER0_CH1  pin (PA9)
  - TIMER0_CH1N pin (PB14)
  - TIMER0_CH2  pin (PA10)
  - TIMER0_CH2N pin (PB15)