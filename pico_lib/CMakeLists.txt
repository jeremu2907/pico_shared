cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_NAME "Shared")

# Pull in the Pico SDK
# include(pico_sdk_import.cmake)
# pico_sdk_init()

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Create a static library target
add_library(${PROJECT_NAME} STATIC
    ${SOURCE_FILES}
)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    pico_stdlib
    hardware_adc
    hardware_i2c

    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
