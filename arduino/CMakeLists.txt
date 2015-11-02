#====================================================================#
# Advanced static library exmaple
#====================================================================#
generate_arduino_library(blink_lib
    SRCS blink_lib.cpp
    HDRS blink_lib.h)

#====================================================================#
# Advanced firwmare example
#====================================================================#
generate_arduino_firmware(blink
    SRCS blink.cpp
    LIBS blink_lib)
