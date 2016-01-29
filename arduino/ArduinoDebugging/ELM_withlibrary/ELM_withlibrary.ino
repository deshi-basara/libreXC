#include <Bridge.h>
#include <SoftwareSerial.h>
#include <elm.h>

#define BYTE  uint8_t
#define BOOL  uint8_t
#define SHORT uint16_t
#define TRUE  1
#define FALSE 0

#define PACK_START_LEN  3
#define PACK_PAY_LEN    2
#define PACK_END_LEN    3
#define BUFF_LEN        6
#define SYNC_BYTE       0x11
#define END_BYTE        0x22

#define linuino     Serial1
#define usb_serial  Serial

BOOL debugging = TRUE;

BYTE buff[BUFF_LEN];
BYTE data[PACK_PAY_LEN];

ELM elm(9,10);

void setup() {
  elm.begin(9600);
  linuino.begin(57600);
  usb_serial.begin(57600);
}

void loop() {
  /*if (linuino.available()) {  
    int data = linuino.read();
    usb_serial.print(String(data, HEX)+" ");
  }*/


  
  receiveData();
  if(checkPacket()) {
    for(SHORT i = 0; i < PACK_PAY_LEN; i++) {
      data[i] = buff[i];
    }
    handleData();
  }
}

void handleData() {
  switch (data[0]) {
    case 0x01: reset(); break;
    case 0x02: available_pids(); break;
    case 0x03: read_all(); break;
    case 0x04: read_pid(data[1]); break;
    case 0x05: read_car(); break;
    case 0x06: read_dct(); break;
    case 0x07: delete_dct(); break;
  }
}

void receiveData() {
  sync();
  for (SHORT i = 0; i < BUFF_LEN; i++) {
    buff[i] = nextByte();
  }
}

void sync() {
  while(TRUE) {
    for(SHORT i = 0; i < PACK_START_LEN; i++) {
      if (nextByte() != SYNC_BYTE) {
        break;
      }
      if (i == PACK_START_LEN-1) {
        return;
      }
    }
  }
}

BYTE nextByte() {
  while(TRUE) {
    if (linuino.available()) {
      return linuino.read();
    }
  }
}

BOOL checkPacket() {
  for(SHORT i = 0; i < PACK_END_LEN; i++) {
    if(buff[BUFF_LEN - 1 - i] != END_BYTE) {
      return FALSE;
    }
  }
  BYTE checksum = 0x00;
  for(SHORT i = 0; i < PACK_PAY_LEN; i++) {
    checksum ^= buff[i];
  }
  if(buff[PACK_PAY_LEN] != checksum) {
    return FALSE;
  }
  return TRUE;
}

void reset() {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: reset()");
}

void available_pids() {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: available_pids()");
}

void read_all() {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: read_all()");
}

void read_pid(int pid) {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: read_pid(0x"+String(pid, HEX)+")");
}

void read_car() {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: read_car()");
}

void read_dct() {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: read_dct()");
}

void delete_dct() {
  if (debugging) usb_serial.println("[Debugging] linuino cmd: delete_dct()");
}

