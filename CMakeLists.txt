cmake_minimum_required(VERSION 3.13)

# Set board type to Pico W BEFORE including SDK
set(PICO_BOARD pico_w)

# This includes the file you copied earlier
include(pico_sdk_import.cmake)

project(pico_bouncing_balls C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()


# Add executable with new modular sources
add_executable(pico_bouncing_balls
    src/main.cpp
    src/BouncingBallsDemo.cpp
    src/Hub75Display.cpp
)

# Allow the compiler to find headers in the 'include' directory
target_include_directories(pico_bouncing_balls PRIVATE include)

# Generate PIO header
pico_generate_pio_header(pico_bouncing_balls ${CMAKE_CURRENT_LIST_DIR}/src/Hub75Display.pio)

# Add project root to include path (for lwipopts.h)
target_include_directories(pico_bouncing_balls PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Pull in standard libraries + WiFi chip support + PIO + Multicore
target_link_libraries(pico_bouncing_balls
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_pio
    pico_multicore
)

# Enable USB output so we can use 'printf' debugging later
pico_enable_stdio_usb(pico_bouncing_balls 1)
pico_enable_stdio_uart(pico_bouncing_balls 0)

# Create the .uf2 file we need for flashing (without picotool metadata)
pico_add_extra_outputs(pico_bouncing_balls)
