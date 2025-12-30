# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/root/pico/pomodoro-timer/build/_deps/picotool-src"
  "/root/pico/pomodoro-timer/build/_deps/picotool-build"
  "/root/pico/pomodoro-timer/build/_deps"
  "/root/pico/pomodoro-timer/build/picotool/tmp"
  "/root/pico/pomodoro-timer/build/picotool/src/picotoolBuild-stamp"
  "/root/pico/pomodoro-timer/build/picotool/src"
  "/root/pico/pomodoro-timer/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/root/pico/pomodoro-timer/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/root/pico/pomodoro-timer/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
