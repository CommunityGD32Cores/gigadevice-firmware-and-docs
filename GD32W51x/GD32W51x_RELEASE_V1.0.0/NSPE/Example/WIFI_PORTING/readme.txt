#############
Porting Guide
#############
    ..
    ├──Demo                just for test.
    ├──Platform            peripherals drivers.
    ├──Port                porting files for different third-party libraries, like LWIP, FreeRTOS, MbedTLS.
    └──Wifi                wifi manager, APIs and driver library.
    1. In Demo, there are four parts as follows:
        ..
        ├──Application     main.c and some demo that shows how to use wifi.
        ├──FreeRTOS        RTOS used in demo project, FreeRTOS V10.3.1.
        ├──RT-Thread       RTOS used in demo project, rt-thread v4.0.3.
        ├──Lwip            Lwip used in demo project, lwip V2.1.2.
        ├──Mbedtls         Mbedtls used in demo Project, based on Mbedtls-2.17.0 and added support for GD32W51x Hardware Accelerator.
        └──Project         demo projects.
        In Project, there are four projects as follows:
            ..
            ├──FreeRTOS_MbedTLS_LWIP         this demo project support FreeRTOS, HW accelerated Mbedtls and LWIP
            ├──FreeRTOS_ROM-MbedTLS_LWIP     this demo project support FreeRTOS, Mbedtls(has implemented in ROM) and LWIP
            ├──RT-Thread_MbedTLS_LWIP        this demo project support RT-Thread, HW accelerated Mbedtls and LWIP
            └──RT-Thread_ROM-MbedTLS_LWIP    this demo project support RT-Thread, Mbedtls(has implemented in ROM) and LWIP
            user can choose different project according to different requirements.
    2. In Platform, there are basic peripherals drivers and boot setting for GD32w51x, and the startup.s and .c file.
    3. In Port, there are five parts ad follows:
        ..
        ├──Bsp             higher level APIs based on the basic peripherals drivers.
        ├──Config          contains platform_def.h can set some special settings for chip or demo board.
        ├──Lwip            contains port files when use Lwip in project.
        ├──Mbedtls         contains only the TLS handshake process，must cowork with ROM MbedTLS which contains bignum, crypto, hash and other algorithms.
        └──OS              porting files for RTOS-FreeRTOS/RTT.
    4. In Wifi, there are all the files when use wifi, user would not change before porting to project; and wifi api in wifi_management.c
       and wifi_netlink.c is recommended to use.

Note:
1. In project.sct files, the start address of SRAM is 0x20000200, cause the first 512B is used for ROM code to put some global variables.
2. ROM MbedTLS can save code size stored in Flash, and can accelerate encryption and decryption operation for wifi.
