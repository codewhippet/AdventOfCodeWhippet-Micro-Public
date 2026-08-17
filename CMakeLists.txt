cmake_minimum_required(VERSION 3.31)

if(NOT PICO_BOARD)
    cmake_policy(SET CMP0194 OLD)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Pico SDK
#

add_library(pico_settings INTERFACE)
if(PICO_BOARD)
    if(WIN32)
        set(USERHOME $ENV{USERPROFILE})
    else()
        set(USERHOME $ENV{HOME})
    endif()
    set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
    include(${picoVscode})
    include(pico_sdk_import.cmake)
    include(pico_settings.cmake)
endif()

#
# Win32 build configuration
#

add_library(win32_settings INTERFACE)
if(NOT PICO_BOARD)
    include(win32_settings.cmake)
endif()

#
# Main target
#

project(AdventOfCodeWhippet C CXX ASM)

if(PICO_BOARD)
    pico_sdk_init()
endif()

add_executable(AdventOfCodeWhippet
    AdventOfCodeWhippet.cpp
    )

#
# Common library dependencies
#

add_subdirectory(MicroUtilities)
add_subdirectory(Utilities)
add_subdirectory(AoC2015)
add_subdirectory(AoC2016)
add_subdirectory(AoC2017)
add_subdirectory(AoC2018)
add_subdirectory(AoC2019)

target_link_libraries(AdventOfCodeWhippet PRIVATE MicroUtilities)
target_link_libraries(AdventOfCodeWhippet PRIVATE Utilities)
target_link_libraries(AdventOfCodeWhippet PRIVATE AoC2015)
target_link_libraries(AdventOfCodeWhippet PRIVATE AoC2016)
target_link_libraries(AdventOfCodeWhippet PRIVATE AoC2017)
target_link_libraries(AdventOfCodeWhippet PRIVATE AoC2018)
target_link_libraries(AdventOfCodeWhippet PRIVATE AoC2019)

#
# More Pico SDK
#

if(PICO_BOARD)
    pico_enable_stdio_uart(AdventOfCodeWhippet 0)
    pico_enable_stdio_usb(AdventOfCodeWhippet 1)

    target_link_libraries(AdventOfCodeWhippet PUBLIC pico_stdlib)

    pico_add_extra_outputs(AdventOfCodeWhippet)
endif()

#
# Pico flashing
#

if(PICO_BOARD)
    find_program(OPENOCD_EXECUTABLE openocd
        HINTS "$ENV{USERPROFILE}/.pico-sdk/openocd/${openocdVersion}"
    )

    if(PICO_PLATFORM STREQUAL "rp2040")
        set(OPENOCD_TARGET "rp2040")
    elseif(PICO_PLATFORM MATCHES "^rp2350")
        set(OPENOCD_TARGET "rp2350")
    else()
        message(WARNING "Unknown PICO_PLATFORM [${PICO_PLATFORM}]")
    endif()

    if(OPENOCD_EXECUTABLE AND OPENOCD_TARGET)
        get_filename_component(OPENOCD_DIR ${OPENOCD_EXECUTABLE} DIRECTORY)
        set(OPENOCD_SCRIPTS_DIR "${OPENOCD_DIR}/scripts")
        add_custom_target(Flash
            COMMAND ${OPENOCD_EXECUTABLE}
                -s "${OPENOCD_SCRIPTS_DIR}"
                -f interface/cmsis-dap.cfg
                -f target/${OPENOCD_TARGET}.cfg
                -c "adapter speed 5000"
                -c "program $<TARGET_FILE:AdventOfCodeWhippet> verify reset exit"
            DEPENDS AdventOfCodeWhippet
            USES_TERMINAL
            COMMENT "Flashing via OpenOCD..."
            )
    else()
        message(WARNING "OpenOCD not found")
    endif()
endif()
