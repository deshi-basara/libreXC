# libreXC

Hier kommt später die Beschreibung hin.

## arduino cmake

### Systemvoraussetzungen 

* Allgemeine Voraussetzungen:

  - ``CMake`` - http://www.cmake.org/cmake/resources/software.html
  - ``Arduino SDK`` - http://www.arduino.cc/en/Main/Software

* Zusätzlich für Linux:

  - ``gcc-avr``      - AVR GNU GCC compiler
  - ``binutils-avr`` - AVR binary tools
  - ``avr-libc``     - AVR C library
  - ``avrdude``      - Firmware uploader

### Einrichtung und Benutzung

Kurz zusammengefasst müssen folgende Befehle ausgeführt werden:

    mkdir build
    cd build
    cmake -D ARDUINO_DEFAULT_PORT=/dev/tty.usbmodem1411 -D ARDUINO_DEFAULT_BOARD uno -G"Xcode" ..
    make
    make upload              # um die firmware auf das Arduino zu laden [optional]

Beim cmake-Befehl muss der Port entsprechend an das Betriebssystem und das verwendete Board angepasst werden. Der Port eines angeschlossenen Arduinos kann in der Arduino IDE über *Tools* &rarr; *Serieller Port* ausgelesen werden. 
Über ``cmake -G"[Generator]"`` kann der Generator gewählt werden. Mit ``cmake -G`` wird eine Liste der verfügbaren Generatoren angezeigt.

Wenn der cmake-Befehl einmal ausgeführt ist, reicht fortan der make-Befehl (auch bei Änderungen an dem CMakeLists.txt's).

Das build-Verzeichnis ist über einen Eintrag in der .gitignore-Datei geschützt. Bitte nur in dieses Verzeichnis hinein kompilieren.