#include <Bridge.h>
#include <SoftwareSerial.h>
#include <elm.h>

#define PACK_LEN    2
#define byte        uint8_t

byte buff[PACK_LEN];
byte lastCmd;

ELM elm(9,10);

void setup() {
  elm.begin(9600); //Serial ELM  
  Bridge.begin(); // make contact with linuino
}

void loop() {
  Bridge.get("cmd",buff,PACK_LEN);
  switch (buff[0]) {
    case 0x01: reset(); break;
    case 0x02: available_pids(); break;
    case 0x03: read_all(); break;
    case 0x04: read_pid(buff[1]); break;
    case 0x05: read_car(); break;
    case 0x06: read_dct(); break;
    case 0x07: delete_dct(); break;
    default: delay(10);
  }
}

void reset() {

}

void available_pids() {

}

void read_all() {

}

void read_pid(int pid) {

}

void read_car() {

}

void read_dct() {

}

void delete_dct() {

}

