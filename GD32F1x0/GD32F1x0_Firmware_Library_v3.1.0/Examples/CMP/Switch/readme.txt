/*!
    \file  readme.txt
    \brief description of the comparator switch demo
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

  This demo is based on the GD32150R-EVAL/GD32190R-EVAL board, it shows how to
configure the comparator input switch. In this demo, input 3.3V(GD32150R) or 5V
(GD32190R) to PA4 ,turn on comparator input switch,then PA4 will be connected to 
CMP1_IP.After system start-up, enable comparator,then LED2 is on.

  Connect PA4 to +3V3(GD32150R) or +5V(GD32190R). 
  Connect PA6 to PC11(GD32150R) or PA12(GD32190R).   
