/*!
    \file  hw_config.c
    \brief set the vector table base address
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0.h"
#include "hw_config.h"

/**
  * @brief  Set the vector table base address.
  * @param  None
  * @retval None
  */
void interrupt_config(void)
{ 
    /* Set the vector table base address at 0x08000000 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x0000);
}
