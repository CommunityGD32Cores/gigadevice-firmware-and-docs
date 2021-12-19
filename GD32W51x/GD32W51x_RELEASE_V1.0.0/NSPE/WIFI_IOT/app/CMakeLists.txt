
cmake_minimum_required(VERSION 3.15)

add_library(app)

target_sources(app
    PRIVATE
        atcmd.c
        cmd_shell.c
        console.c
        main.c
    )

target_sources(app
    PRIVATE
        ../demo/iperf3_main.c
        ../demo/ota_demo.c
        ../demo/ssl_certs.c
        ../demo/ssl_client.c
        ../demo/tcp_test.c
        ../demo/udp_test.c
        ../demo/cmsis_os_demo.c
    )

target_link_libraries(app
    gd32w51x_peripheral_api
    gd32w51x_wifi_api
    bsp_api
    common_api
    lwIP_api
    mbedtls_ssl_api
    wifi_api
    os_api
    cmsis_api
    $<$<BOOL:${CONFIG_ALICLOUD_SUPPORT}>:alicloud>
    $<$<BOOL:${CONFIG_IPERF_TEST}>:iperf3>
    $<$<BOOL:${CONFIG_FATFS_SUPPORT}>:fatfs>
    )

target_sources(app
    PRIVATE
        _build_date.h
)

add_custom_command(OUTPUT _build_date.h
    COMMAND ${TAGET_PROJECT_DIR}/build_date.bat ${PROJECT_SOURCE_DIR}/NSPE
)