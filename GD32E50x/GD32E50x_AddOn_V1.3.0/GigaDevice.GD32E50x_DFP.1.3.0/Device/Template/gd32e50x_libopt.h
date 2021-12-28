/*
    Copyright (C) 2020 GigaDevice

    \version 2020-04-01, V1.0.0, firmware for GD32E50x
*/

#ifndef GD32E50X_LIBOPT_H
#define GD32E50X _LIBOPT_H

/* Comment the line below to disable peripheral header file inclusion */
#include "RTE_Components.h"

#ifdef RTE_DEVICE_STDPERIPHERALS_ADC
#include "gd32e50x_adc.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_BKP
#include "gd32e50x_bkp.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_CAN
#include "gd32e50x_can.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_CRC
#include "gd32e50x_crc.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_CTC
#include "gd32e50x_ctc.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_DAC
#include "gd32e50x_dac.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_DBG
#include "gd32e50x_dbg.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_DMA
#include "gd32e50x_dma.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_EXMC
#include "gd32e50x_exmc.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_EXTI
#include "gd32e50x_exti.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_FMC
#include "gd32e50x_fmc.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_FWDGT
#include "gd32e50x_fwdgt.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_GPIO
#include "gd32e50x_gpio.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_SHRTIMER
#include "gd32e50x_shrtimer.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_I2C
#include "gd32e50x_i2c.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_MISC
#include "gd32e50x_misc.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_PMU
#include "gd32e50x_pmu.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_RCU
#include "gd32e50x_rcu.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_RTC
#include "gd32e50x_rtc"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_SPI
#include "gd32e50x_spi.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_TIMER
#include "gd32e50x_timer.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_USART
#include "gd32e50x_usart.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_WWDGT
#include "gd32e50x_wwdgt.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_SQPI
#include "gd32e50x_sqpi.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_ENET
#include "gd32e50x_enet.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_TMU
#include "gd32e50x_tmu.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_CMP
#include "gd32e50x_cmp.h"
#endif
#ifdef RTE_DEVICE_STDPERIPHERALS_SDIO
#include "gd32e50x_sdio.h"
#endif

#endif /* GD32E50X_LIBOPT_H */
