cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_NAME "Shared")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern)

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
    hardware_pwm
)

if((PICO_BOARD STREQUAL "pico_w") OR (PICO_BOARD STREQUAL "pico2_w"))
    target_link_libraries(${PROJECT_NAME} PRIVATE
        pico_lwip_nosys
        pico_cyw43_arch_lwip_threadsafe_background

        Extern
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/extern/include
)
