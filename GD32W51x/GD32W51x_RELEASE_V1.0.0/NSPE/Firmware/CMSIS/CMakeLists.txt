
cmake_minimum_required(VERSION 3.15)

add_library(cmsis_api INTERFACE)

add_library(cmsis)

target_sources(cmsis
    PRIVATE
        GD/GD32W51x/Source/system_gd32w51x.c
)

target_sources(cmsis
    PRIVATE
        GD/GD32W51X/SOURCE/GCC/startup_gd32w51x.s
)

target_sources(cmsis
    PUBLIC
        DSP_Lib_v1.6.0/source/arm_bitreversal.c
        DSP_Lib_v1.6.0/source/arm_bitreversal2.S
        DSP_Lib_v1.6.0/source/arm_cfft_f32.c
        DSP_Lib_v1.6.0/source/arm_cfft_radix2_f32.c
        DSP_Lib_v1.6.0/source/arm_cfft_radix4_f32.c
        DSP_Lib_v1.6.0/source/arm_cfft_radix8_f32.c
        DSP_Lib_v1.6.0/source/arm_common_tables.c
        DSP_Lib_v1.6.0/source/arm_const_structs.c
        DSP_Lib_v1.6.0/source/arm_cos_f32.c
        DSP_Lib_v1.6.0/source/arm_sin_cos_f32.c
        DSP_Lib_v1.6.0/source/arm_sin_f32.c
)

target_include_directories(cmsis_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/GD/GD32W51x/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DSP_Lib_v1.6.0/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ARM/cmsis>
)

target_link_libraries(cmsis
    cmsis_api
    gd32w51x_peripheral_api
)
