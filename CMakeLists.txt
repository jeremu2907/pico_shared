cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(/root/pico-sdk/external/pico_sdk_import.cmake)

project(Main C CXX ASM)

pico_sdk_init()

add_subdirectory(../pico_lib pico_lib_build)

add_executable(Main
    main.cpp
)

pico_set_program_name(Main "Main")
pico_set_program_version(Main "0.1")

pico_enable_stdio_uart(Main 1)
pico_enable_stdio_usb(Main 1)

target_link_libraries(Main PRIVATE pico_stdlib)

if((PICO_BOARD STREQUAL "pico_w") OR (PICO_BOARD STREQUAL "pico2_w"))
    target_link_libraries(Main PRIVATE
        pico_cyw43_arch_lwip_threadsafe_background
    )
endif()

target_link_libraries(Main PRIVATE Shared)

target_include_directories(Main PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/pico_lib/includes
)

pico_add_extra_outputs(Main)