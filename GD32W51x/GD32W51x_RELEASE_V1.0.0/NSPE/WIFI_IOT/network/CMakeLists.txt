cmake_minimum_required(VERSION 3.15)

if (CONFIG_IPERF_TEST)
add_subdirectory(iperf3)
endif()

add_subdirectory(lwip-2.1.2)
add_subdirectory(mbedtls-2.17.0-ssl)
