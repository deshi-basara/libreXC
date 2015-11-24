set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain
cmake_minimum_required(VERSION 2.8)

#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(libreXC C CXX)

include(CMakeConfig.txt)

#print_board_list()
#print_programmer_list()

add_subdirectory(arduino)   #add the arduino directory into build
