cmake_minimum_required(VERSION 3.13)

# Define the project and its supported languages
project(SharedLib C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in the Pico SDK
include(pico_sdk_import.cmake)
pico_sdk_init()

# Gather all source files
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Create a static library target
add_library(SharedLib STATIC
    ${SOURCE_FILES}
)

# Link against required Pico libraries
target_link_libraries(SharedLib
    pico_stdlib
    pico_cyw43_arch_none
    hardware_adc
)

# Include directory for headers
target_include_directories(SharedLib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
