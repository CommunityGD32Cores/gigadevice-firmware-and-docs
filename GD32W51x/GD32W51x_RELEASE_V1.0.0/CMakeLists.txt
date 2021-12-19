cmake_minimum_required(VERSION 3.15)
set (CMAKE_C_COMPILER_FORCED true)
project(GD32W51X LANGUAGES C ASM)

set(CONFIG_TZ_ENABLED              OFF           CACHE BOOL      "Whether to enable trustzone")
set(CONFIG_OS                      "FREERTOS"    CACHE STRING    "Select which OS")
set(CONFIG_WLANLIB_BUILDED         ON            CACHE BOOL      "Whether wlan lib builded")

include(${CMAKE_TOOLCHAIN_FILE})
toolchain_reload_compiler()

include_directories(
    config
    NSPE/Firmware/CMSIS/ARM/cmsis
    NSPE/Firmware/CMSIS/GD/GD32W51x/Include
    NSPE/Firmware/CMSIS/DSP_Lib_v1.6.0/include
    NSPE/WIFI_IOT/app
    NSPE/WIFI_IOT/bsp
    NSPE/WIFI_IOT/common
    MBL/platform
    ROM-EXPORT/platform
    ROM-EXPORT/source
    ROM-EXPORT/mbedtls-2.17.0-rom/include
)

if (CONFIG_TZ_ENABLED)
    add_subdirectory(MBL/source)
    include_directories(MBL/source)
else()
    include_directories(MBL/source_ns)
    add_subdirectory(MBL/source_ns)
endif()


add_subdirectory(NSPE)
