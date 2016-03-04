#libreXC
**Contributors:** [deshi-basara](https://github.com/deshi-basara), [vpav](https://github.com/vpav), [florianbeck](https://github.com/florianbeck)   
**Organisation:** Stuttgart Media University  
**License:** GPLv3 ([http://www.gnu.org/licenses/gpl-3.0.html](http://www.gnu.org/licenses/gpl-3.0.html))

A free, open car hacking tool for reading vehicle data via Arduino.

## Description
`libreXC` is a free, open car hacking tool for reading vehicle data via Arduino Yún. The project is consisting of two parts:

1. [arduino](arduino) — for reading and parsing vehicle information with an ELM chip
2. [linuino](linuino) — reading and logging the data from arduino via websocket

**Note:** The Arduino-Part uses `libELM`, which ist also part of this project, but hosted in a [separate repository](https://github.com/deshi-basara/libELM).


## Recommended Hardware
* [Arduino Yun](https://www.arduino.cc/en/Main/ArduinoBoardYun)
* [SparkFun OBD-II UART](https://www.sparkfun.com/products/9555)

## Licence & Copyright

libreXC was written by Simon Schuster, Florian Beck and Viktor Pavlovic.
libreXC is licensed under the [GNU General Public License v3](http://www.gnu.org/licenses/gpl-3.0). 