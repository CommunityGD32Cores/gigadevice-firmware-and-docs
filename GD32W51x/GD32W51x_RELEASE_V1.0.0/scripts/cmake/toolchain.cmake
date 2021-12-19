
set(CMAKE_SYSTEM_NAME Generic)
set(CROSS_COMPILE arm-none-eabi)
find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}-gcc)
find_program(CMAKE_CXX_COMPILER ${CROSS_COMPILE}-g++)

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(LINKER_VENEER_OUTPUT_FLAG -Wl,--cmse-implib,--out-implib=)
set(COMPILER_CMSE_FLAG -mcmse)


set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/set_extensions.cmake)



if (CONFIG_TZ_ENABLED)
    message(FATAL_ERROR "Not Support TrustZone Enable Yet!")
    # add_definitions(
    #     -DCONFIG_TZ_ENABLED
    # )
endif()

if (CONFIG_OS STREQUAL "FREERTOS")
    set(PLATFORM_OS_FREERTOS ON)
    add_definitions(
        -DPLATFORM_OS_FREERTOS
    )
endif()

# Set architecture and CPU
set(SYSTEM_PROCESSOR cortex-m33)
set(SYSTEM_ARCHITECTURE armv8-m.main)
set(SYSTEM_FP ON)


set(CMAKE_SYSTEM_PROCESSOR ${SYSTEM_PROCESSOR})
set(CMAKE_SYSTEM_ARCHITECTURE ${SYSTEM_ARCHITECTURE})


macro(toolchain_reset_compiler_flags)
    set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "")

    add_compile_options(
        --specs=nano.specs
        -Wall
        -Wno-format
        -Wno-return-type
        -Wno-unused-but-set-variable
        -Wno-address-of-packed-member
        -Wno-unused-variable
        -Wno-maybe-uninitialized
        -Wno-unused-function
        -c
        -fdata-sections
        -ffunction-sections
        -fno-builtin
        -fno-short-enums
        -funsigned-char
        -mthumb
        -nostdlib
        -std=c99
        -g
        -O2
    )
endmacro()

macro(toolchain_reset_linker_flags)
    set_property(DIRECTORY PROPERTY LINK_OPTIONS "")

    add_link_options(
        --entry=Reset_Handler
        --specs=nano.specs
        -u _printf_float
        LINKER:-check-sections
        LINKER:-fatal-warnings
        LINKER:--gc-sections
        LINKER:--no-wchar-size-warning
        LINKER:--print-memory-usage
        LINKER:-no-enum-size-warning
    )

endmacro()

macro(toolchain_set_processor_arch)
    set(CMAKE_SYSTEM_PROCESSOR ${SYSTEM_PROCESSOR})
    set(CMAKE_SYSTEM_ARCHITECTURE ${SYSTEM_ARCHITECTURE})
endmacro()

macro(toolchain_reload_compiler)
    toolchain_set_processor_arch()
    toolchain_reset_compiler_flags()
    toolchain_reset_linker_flags()

    unset(CMAKE_C_FLAGS_INIT)
    unset(CMAKE_ASM_FLAGS_INIT)

    set(CMAKE_C_FLAGS_INIT "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_FLAGS_INIT "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_C_LINK_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_LINK_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
if (DEFINED SYSTEM_FP)
        if(SYSTEM_FP)
            string(APPEND CMAKE_C_FLAGS_INIT " -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
            string(APPEND CMAKE_ASM_FLAGS_INIT " -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
            string(APPEND CMAKE_C_LINK_FLAGS " -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
            string(APPEND CMAKE_ASM_LINK_FLAGS " -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
        endif()
endif()
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_INIT})
    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS_INIT})
endmacro()

toolchain_reload_compiler()

macro(target_add_scatter_file target)
    target_link_options(${target}
        PRIVATE
        -T $<TARGET_OBJECTS:${target}_scatter>
    )

    add_dependencies(${target}
        ${target}_scatter
    )

    add_library(${target}_scatter OBJECT)
    foreach(scatter_file ${ARGN})
        target_sources(${target}_scatter
            PRIVATE
                ${scatter_file}
        )
        # Cmake cannot use generator expressions in the
        # set_source_file_properties command, so instead we just parse the regex
        # for the filename and set the property on all files, regardless of if
        # the generator expression would evaluate to true or not.
        string(REGEX REPLACE ".*>:(.*)>$" "\\1" SCATTER_FILE_PATH "${scatter_file}")
        set_source_files_properties(${SCATTER_FILE_PATH}
            PROPERTIES
            LANGUAGE C
        )
    endforeach()



    target_compile_options(${target}_scatter
        PRIVATE
            -E
            -P
            -xc
    )
endmacro()


