set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain
cmake_minimum_required(VERSION 2.8)

#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(libreXC C CXX)

include(CMakeConfig.txt)

generate_arduino_firmware(librexc
    SKETCH src)