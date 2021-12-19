
cmake_minimum_required(VERSION 3.15)

add_library(alicloud_api INTERFACE)

add_library(alicloud)

SET(ali_linkkit_src ${CMAKE_CURRENT_SOURCE_DIR}/iotkit-embedded-3.2.0/lib_iot_sdk_src)


file(GLOB_RECURSE SRC_SDK  ${ali_linkkit_src}/eng/*.c)

list(FILTER SRC_SDK EXCLUDE REGEX "${ali_linkkit_src}/eng/wrappers/external_libs/*")


MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list  ${ali_linkkit_src}/eng/*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(header_list)
list(FILTER header_list EXCLUDE REGEX "${ali_linkkit_src}/eng/wrappers/external_libs/*")

target_sources(alicloud
    PRIVATE
        alicloud_entry.c
        linkkit_example_solo.c
        ${SRC_SDK}
)

target_include_directories(alicloud
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../app
)

target_include_directories(alicloud_api
    INTERFACE
        ${header_list}
        ${ali_linkkit_src}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(alicloud
    alicloud_api
    os_api
    gd32w51x_peripheral_api
    gd32w51x_wifi_api
    mbedtls_ssl
    lwIP_api
    wifi_api
)
