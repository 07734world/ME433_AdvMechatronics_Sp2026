# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(uart_boot C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(uart_boot
    uart_boot.c
    )

# pull in common dependencies
target_link_libraries(uart_boot pico_stdlib hardware_flash)

# add partition table
pico_embed_pt_in_binary(uart_boot ${CMAKE_CURRENT_LIST_DIR}/uart-pt.json)

# create absolute UF2, as it's a bootloader so shouldn't go in a partition
pico_set_uf2_family(uart_boot "absolute")

# create map/bin/hex file etc.
pico_add_extra_outputs(uart_boot)

# add url via pico_set_program_url


# Create separate binary to be loaded onto other device
add_executable(uart_binary
    uart_binary.c
    )

# pull in common dependencies
target_link_libraries(uart_binary pico_stdlib)

pico_set_binary_type(uart_binary no_flash)

# package uf2 in flash
pico_package_uf2_output(uart_binary)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(uart_binary)

# call pico_set_program_url to set path to example on github, so users can find the source for an example via picotool
