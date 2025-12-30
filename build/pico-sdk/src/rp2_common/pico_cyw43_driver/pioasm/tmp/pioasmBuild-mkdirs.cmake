# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/root/pico/pico-sdk/tools/pioasm"
  "/root/pico/pomodoro-timer/build/pioasm"
  "/root/pico/pomodoro-timer/build/pioasm-install"
  "/root/pico/pomodoro-timer/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/root/pico/pomodoro-timer/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/root/pico/pomodoro-timer/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/root/pico/pomodoro-timer/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/root/pico/pomodoro-timer/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/root/pico/pomodoro-timer/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
