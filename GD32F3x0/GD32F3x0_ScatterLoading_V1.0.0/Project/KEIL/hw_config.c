/*!
    \file  hw_config.c
    \brief set the vector table base address
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0.h"
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
