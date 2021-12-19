cmake_minimum_required(VERSION 3.15)

add_library(os_api INTERFACE)
add_library(os)

if (PLATFORM_OS_FREERTOS)
    add_subdirectory(FreeRTOSv10.3.1)
    target_include_directories(os_api
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOSv10.3.1/Source/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOSv10.3.1/Source/portable/GCC/ARM_CM33_NTZ/non_secure>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOSv10.3.1/CMSIS/RTOS2/FreeRTOS/Include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOSv10.3.1/CMSIS/RTOS2/FreeRTOS/Source>
    )
endif()



target_include_directories(os_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_sources(os
    PRIVATE
        ../common/wrapper_os.c
)

target_link_libraries(os
    $<$<BOOL:${PLATFORM_OS_FREERTOS}>:freertos>
    os_api
)