# ELM library

## Getting started

Setting up the ELM library is fairly easy. 

**Note: This library uses SoftwareSerial to connect to the ELM chip. SoftwareSerial is not available on all Arduino pins!**

```cpp
ELM myELM;

int serialRX = 8;
int serialTX = 9;

myELM.begin(serialRX, serialTX);
```

## Reading a PID

PIDs range from 0 to 255 and each one represents a specific sensor value.
A list of PIDs is available here: https://en.wikipedia.org/wiki/OBD-II_PIDs
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

**TODO**

## Reading DTCs

**TODO**

## Clearing DTCs

**TODO**

## Reading Vehicle Information

**TODO**

## Limitations

This library has various limitations. 

* Error Handling
* Mode 1 (not all PIDs supported)
* Mode 2 (not implemented)
* Mode 5,6,7,8 (not implemented)
* Mode 9 (not all Mode 9 PIDs are supported)
* Mode 10 (not implemented)
