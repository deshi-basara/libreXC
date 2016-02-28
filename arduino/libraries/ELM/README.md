# ELM library

ELM library is an Arduino library which handles communication with an [ELM327](http://www.elmelectronics.com/obdic.html) or ELM327 compatible chip used for car on-board diagnosis. It supports the display of current data (OBD Mode 1) and the display and clearing of diagnostic trouble codes (DTCs). Furthermore it is capable of displaying vehicle information like the [VIN](https://en.wikipedia.org/wiki/Vehicle_identification_number) and the ECU Model.

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
All values come in the correct parsed form so you can use them directly (with some limits, see "Code Size" below). 
The values are returned as Strings however, so you might need to convert them to int, float or whatever you need.


```cpp
byte pid = 0x05;
String myValue = myELM.get_pid_data(pid);
String myDescription = myELM.get_pid_desc(pid);
String myUnit = myELM.get_pid_unit(pid);

Serial.println("The value of " + myDesc + " is " + myValue + " "  + myUnit); 
```

## Reading all supported PIDs

`get_available_pids()` can be used to get the values of all supported PIDs  It returns a comma-separated list.

```cpp
String supportedPIDs = myELM.get_supported_pids();
Serial.println("PIDs supported: " + supportedPIDs);
```


## List of supported PIDs

Each ECU supports different PIDs. To check if a specific PID is supported, `pid_available()` can be used. This function is for for informative purposes only. The lib checks for PID support on startup and allows only supported PIDs to be retrieved.

```cpp
boolean rpm_supported = myELM.pid_available(0x0C);
if(rpm_supported) {
  Serial.println("This ECU supports display of engine RPM");
}
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

## Reset

You can reboot the ELM chip using the `reset()` function. Please note: The reset can take some time, the function will wait *up to 5 seconds* and check if the reboot was successful.

```cpp
if(myELM.reset()) {
  Serial.println("Reboot successful");
} else {
  Serial.println("Error during reboot.");
};
```

## Constants

Most of the functions return either a string or a boolean value. In all cases (except `pid_available()`) the value can be checked for errors. Functions with boolean return type return the error value directly. String functions can be checked against the ERROR-Constant.

```cpp
String raw = myELM.get_pid_rawdata(0x0C);
if(raw.startsWith(ELM.ERROR)){
  Serial.println("Sorry, could not retrieve rpm data, the error is: " + raw);
} else {
  Serial.println("Engine rpm data is: " + raw);
}
```


## Limitations

This library has various limitations. 

### Code Size

Unfortunately the lib, especially the private function `parsePID()` is pretty large in size. `parsePID()` is responsible for parsing the received raw data and translating it into a human readable format. The lib currently supports ~75 different PIDs. Almost each PID requires a different formula, has a different description and the value has a different unit. This results in a total of **~30k** compiled code. This is too big to fit in the memory of the smaller Arduino types (Uno, Micro, Mini, Leonardo, Yun, ...). Therefore, on these plattforms the lib only parses the *most important* PIDs and returns the rest unparsed. Full parsing is supported on [Arduino Mega](https://www.arduino.cc/en/Main/ArduinoBoardMega2560).   

### OBD mode support

OBD Mode | Description | Support
------------ | ------------- | -------------
Mode 1 | Current data | :white_check_mark: **Supported** (not all PIDs supported; code size limitations, see above)
Mode 2 | Freeze frame data | :x: Not supported
Mode 3 | Display stored DTCs | :white_check_mark: **Supported**, but no parsing of data
Mode 4 | Clear DTCs | :white_check_mark: **Supported**
Mode 5 | Oxygen sensor monitoring test results | :x: Not supported
Mode 6 | other component/system monitoring | :x: Not supported
Mode 7 | Display pending DTCs | :x: Not supported
Mode 8 | Control operation of on-board component/system | :x: Not supported
Mode 9 | Vehicle information | :o: **Partially** (VIN and ECU name)
Mode 10 | Display permanent DTCs | :x: Not supported

### Other limitations

* Error Handling
* Only automatic protocol detection, no manual protocol setting


**TODO: desciption**

## Licence & Copyright

ELM lib was written by Simon Schuster, Florian Beck and Viktor Pavlovic.
ELM lib is licensed under the [GNU General Public License v3](http://www.gnu.org/licenses/gpl-3.0). 
