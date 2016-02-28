# ELM library

## Getting started

Setting up the ELM library is fairly easy. 

**Note: This library uses SoftwareSerial to connect to the ELM chip. SoftwareSerial is not available on all Arduino pins! See the [Arduino Reference](https://www.arduino.cc/en/Reference/SoftwareSerial) for more information**

```cpp
ELM myELM;

int serialRX = 8;
int serialTX = 9;

myELM.begin(serialRX, serialTX);
```

## Reading a PID

PIDs range from 0 to 255 and each one represents a specific sensor value.
A list of PIDs is available on [Wikipedia](https://en.wikipedia.org/wiki/OBD-II_PIDs).
Values for each PID get returned in a raw and partially unreadable form. 
Luckily this library does the dirty job for you so you don't get your hands dirty. 
All values come in the correct parsed form so you can use them directly.
The values are returned as Strings however, so you might need to convert them to int, float or whatever you need.

```cpp
byte pid = 0x05;
String myValue = myELM.get_pid_data(pid);
String myDescription = myELM.get_pid_desc(pid);
String myUnit = myELM.get_pid_unit(pid);

Serial.println("The value of " + myDesc + " is " + myValue + " "  + myUnit); 
```

## Reading all supported PIDs

Each ECU supports different PIDs. To get a list of supported PIDs, `get_supported_pids()` can be used. It returns a comma-separated list. This function is for for informative purposes only. The lib checks for PID support on startup and allows only supported PIDs to be retrieved.

```cpp
String supportedPIDs = myELM.get_supported_pids();
Serial.println("PIDs supported: " + supportedPIDs);
```

## Reading DTCs

This function returns the diagnostic trouble codes (DTCs) stored in the ECU.
The DTCs are returned in raw form. DTC parsing will be added in a later Version.

```cpp
String raw_dtcs = myELM.get_dtc();

Serial.println(raw_dtcs);
```


## Clearing DTCs

This function clears all stored DTCs in the ECU. It returns `true` on successful execution and `false` on errors.

```cpp
boolean is_cleared = myELM.clear_dtc();

if(is_cleared) {
  Serial.println("DTCs cleared.");
} else {
  Serial.println("Oops, an error occured.");
}

```


## Reading Vehicle Information

**TODO describe single functions/fields**

```cpp
String vin = myELM.get_vin();
String ecu = myELM.get_ecu();
String voltage = myELM.get_voltage();
String protocol = myELM.get_protocol();

Serial.println("vin: " + vin + ", ecu: " + ecu + ", voltage: " + voltage + ", protocol: " + protocol);
```

## Limitations

This library has various limitations. 

### Code Size

Unfortunately the lib, especially the private function `parsePID()` is pretty large in size. `parsePID()` is responsible for parsing the received raw data and translating it into a human readable format. The lib currently supports ~75 different PIDs. Almost each PID requires a different formula, has a different description and the value has a different unit. This results in a total of **~30k** compiled code. This is too big to fit in the memory of the smaller Arduino types (Uno, Micro, Mini, Leonardo, Yun, ...). Therefore, on these plattforms the lib only parses the *most important* PIDs and returns the rest unparsed. Full parsing is supported on [Arduino Mega](https://www.arduino.cc/en/Main/ArduinoBoardMega2560).   

### Other limitations

* Error Handling
* Mode 1 (not all PIDs supported)
* Mode 2 (not implemented)
* Mode 5,6,7,8 (not implemented)
* Mode 9 (not all Mode 9 PIDs are supported)
* Mode 10 (not implemented)
* 

## Licence & Copyright

ELM lib was written by Simon Schuster, Florian Beck and Viktor Pavlovic.
ELM lib is licensed under the [GNU General Public License v3](http://www.gnu.org/licenses/gpl-3.0). 
