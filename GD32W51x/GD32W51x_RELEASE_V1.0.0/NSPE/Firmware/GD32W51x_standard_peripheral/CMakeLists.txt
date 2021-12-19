
cmake_minimum_required(VERSION 3.15)

add_library(gd32w51x_peripheral_api INTERFACE)

add_library(gd32w51x_peripheral)

target_sources(gd32w51x_peripheral
    PRIVATE
        Source/gd32w51x_adc.c
        Source/gd32w51x_cau.c
        Source/gd32w51x_cau_aes.c
        Source/gd32w51x_cau_des.c
        Source/gd32w51x_cau_tdes.c
        Source/gd32w51x_crc.c
        Source/gd32w51x_dbg.c
        Source/gd32w51x_dci.c
        Source/gd32w51x_dma.c
        Source/gd32w51x_efuse.c
        Source/gd32w51x_exti.c
        Source/gd32w51x_fmc.c
        Source/gd32w51x_fwdgt.c
        Source/gd32w51x_gpio.c
        Source/gd32w51x_hau.c
        Source/gd32w51x_hau_sha_md5.c
        Source/gd32w51x_hpdf.c
        Source/gd32w51x_i2c.c
        Source/gd32w51x_icache.c
        Source/gd32w51x_misc.c
        Source/gd32w51x_pkcau.c
        Source/gd32w51x_pmu.c
        Source/gd32w51x_qspi.c
        Source/gd32w51x_rcu.c
        Source/gd32w51x_rtc.c
        Source/gd32w51x_sdio.c
        Source/gd32w51x_spi.c
        Source/gd32w51x_sqpi.c
        Source/gd32w51x_syscfg.c
        Source/gd32w51x_timer.c
        Source/gd32w51x_trng.c
        Source/gd32w51x_tsi.c
        Source/gd32w51x_tzpcu.c
        Source/gd32w51x_usart.c
        Source/gd32w51x_wwdgt.c
)

target_include_directories(gd32w51x_peripheral_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
)

target_link_libraries(gd32w51x_peripheral
    gd32w51x_peripheral_api
)
