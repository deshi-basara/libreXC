// For testing the ELM Shield with Arduino Serial Monitor
// Type AT Commands in Serial Monitor

#include <SoftwareSerial.h>

SoftwareSerial elm(9,10);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  elm.begin(9600);
}

void loop() {
  while (elm.available() > 0) {
    int ret = elm.read();
    if (ret==13) {
      Serial.println();
    } else {
      Serial.write(ret);
    }
  }
  while (Serial.available() > 0) {
    int ret2 = Serial.read();
    elm.write(ret2);
    if (Serial.available() == 0) {
      elm.write(13);
    }
  }
}
