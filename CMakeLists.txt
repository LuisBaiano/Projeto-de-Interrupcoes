# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(pico_sdk_import.cmake)

project(interruption C CXX ASM)

pico_sdk_init()

add_executable(interruption
    interruption.c
)

# Processa o arquivo PIO
pico_generate_pio_header(interruption ${CMAKE_CURRENT_LIST_DIR}/interruption.pio)

# Configura o nome do programa e a versão
pico_set_program_name(interruption "interruption")
pico_set_program_version(interruption "0.1")

pico_enable_stdio_uart(interruption 0)
pico_enable_stdio_usb(interruption 1)

# Linka as bibliotecas necessárias
target_link_libraries(interruption
    pico_stdlib
    hardware_pio
    hardware_clocks
    hardware_gpio
)

target_include_directories(interruption PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Adiciona saídas extras
pico_add_extra_outputs(interruption)

