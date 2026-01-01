cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_NAME Main)

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(/root/pico-sdk/external/pico_sdk_import.cmake)

project(${PROJECT_NAME} C CXX ASM)

pico_sdk_init()

add_subdirectory(../pico_lib pico_lib_build)
# add_subdirectory(extern)

add_executable(${PROJECT_NAME}
    main.cpp
)

pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib)

# if((PICO_BOARD STREQUAL "pico_w") OR (PICO_BOARD STREQUAL "pico2_w"))
#     target_link_libraries(${PROJECT_NAME} PRIVATE
#         # pico_cyw43_arch_lwip_threadsafe_background
#         # pico_btstack_ble
#         # pico_btstack_cyw43
#         # pico_cyw43_arch_none
#     )
# endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    # Extern
    Shared
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    # ${CMAKE_CURRENT_LIST_DIR}/extern/include
    ${CMAKE_CURRENT_LIST_DIR}/pico_lib/include
)

pico_add_extra_outputs(${PROJECT_NAME})