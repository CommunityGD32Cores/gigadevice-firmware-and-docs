
cmake_minimum_required(VERSION 3.15)

add_library(mbedtls_ssl_api INTERFACE)

add_library(mbedtls_ssl)


target_sources(mbedtls_ssl
    PRIVATE
        library/debug.c
        library/net_sockets.c
        library/ssl_cache.c
        library/ssl_ciphersuites.c
        library/ssl_cli.c
        library/ssl_cookie.c
        library/ssl_default_conf.c
        library/ssl_self_test.c
        library/ssl_srv.c
        library/ssl_ticket.c
        library/ssl_tls.c
)


target_sources(mbedtls_ssl
    PRIVATE
        ns_interface/mbedtls_ns_function_api.c
        ns_interface/ns_interface.c
)



target_include_directories(mbedtls_ssl_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ns_interface>
)

target_link_libraries(mbedtls_ssl
    mbedtls_ssl_api
    lwIP_api
    os_api
    wifi_api
)
