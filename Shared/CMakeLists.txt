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
add_library(Shared STATIC
    ${SOURCE_FILES}
)

set(BASE_LIBS
    pico_stdlib
    hardware_adc
    hardware_i2c
)

# Link against required Pico libraries
if(PICO_BOARD STREQUAL "pico_w")
    list(APPEND BASE_LIBS pico_cyw43_arch_none)  # or pico_cyw43_arch_none, depending on your use
endif()

# Add the standard library to the build
target_link_libraries(Shared PUBLIC ${BASE_LIBS})

# Include directory for headers
target_include_directories(Shared PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
