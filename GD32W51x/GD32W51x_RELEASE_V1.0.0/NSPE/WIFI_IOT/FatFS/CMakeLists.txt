cmake_minimum_required(VERSION 3.15)

add_library(fatfs_api INTERFACE)

add_library(fatfs)


target_sources(fatfs
    PRIVATE
        fatfs.c
        wear_levelling_flash.c
        src/diskio.c
        src/ff.c
        src/ffsystem.c
        src/ffunicode.c
)

target_include_directories(fatfs
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../app
)

target_include_directories(fatfs_api
    INTERFACE
        src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fatfs
    fatfs_api
    os_api
    gd32w51x_peripheral_api
)
