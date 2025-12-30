cmake_minimum_required(VERSION 3.13)

# Set board type to Pico W BEFORE including SDK
set(PICO_BOARD pico_w)

# This includes the file you copied earlier
include(pico_sdk_import.cmake)

project(pomodoro_timer C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(pomodoro_timer
    src/main.cpp
)

# Add project root to include path (for lwipopts.h)
target_include_directories(pomodoro_timer PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Pull in standard libraries + WiFi chip support (for the Pico W LED)
target_link_libraries(pomodoro_timer 
    pico_stdlib 
    pico_cyw43_arch_lwip_threadsafe_background
)

# Enable USB output so we can use 'printf' debugging later
pico_enable_stdio_usb(pomodoro_timer 1)
pico_enable_stdio_uart(pomodoro_timer 0)

# Create the .uf2 file we need for flashing (without picotool metadata)
pico_add_extra_outputs(pomodoro_timer)
