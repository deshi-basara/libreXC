//#include <Bridge.h>
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
	
	elm.get_pid_data(0x05);
	
  receiveData();
  if(checkPacket()) {
    for(SHORT i = 0; i < PACK_PAY_LEN; i++) {
      data[i] = buff[i];
    }
    handleData();
  }

  /* For testing without linuino
  data[0] = 0x06;
  data[1] = 0x00;
  handleData();
  delay(5000);
  */
}

void handleData() {
  switch (data[0]) {
    case 0x01: reset(); break; // tested, works
    case 0x02: available_pids(); break; // tested, works
    case 0x03: read_all(); break;
    case 0x04: read_pid(data[1]); break;
    case 0x05: read_car(); break; // tested, works
    case 0x06: read_dtc(); break; // tested, works
    case 0x07: delete_dtc(); break; // tested, works
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

String make_json(String _cmd, String _status, String _data = "") {
  String json = "{";

  json += "\"cmd\":\""+_cmd+"\",";
  json += "\"status\":\""+_status+"\"";

  if (_data != "") {
    json += ",\"data\":"+_data;
  }
  
  json += "}";
  
  return json;
}
String make_jsondata_pid(BYTE pid, String value) {
  String data = "{\"pid\":"+(String)pid+",";
  data += "\"value\":\""+value+"\",";
  data += "\"unit\":\""+elm.get_pid_unit(pid)+"\",";
  data += "\"desc\":\""+elm.get_pid_desc(pid)+"\"";
  data += "}";
  return data;
}

void respond(String data) {
  if(debugging) usb_serial.println(data);
  //Bridge.put("data", data);
}

void reset() {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: reset()");
  if (elm.reset()) {
    respond(make_json("reset","ok"));
  } else {
    respond(make_json("reset","error"));
  };
}

void available_pids() {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: available_pids()");
  String data = elm.get_available_pids();
  if (!data.startsWith(elm.ERROR)) {
    respond(make_json("available_pids", "ok", "["+data+"]"));
  } else {
    respond(make_json("available_pids", "error"));
  }
}

void read_all() {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: read_all()");
  String data = "";
  boolean first = true;
  boolean error = false;
  for (int i = 0; i <= 255; i++) {
    if(elm.pid_available(i)) {
      if (first) {first = false;} else {data += ",";}
      String value = elm.get_pid_data(i);
      if (!value.startsWith(elm.ERROR)) {
        data += make_jsondata_pid(i,value);
      } else {
         error = true;
         break;
      }
    }
  }
  if (!error) {
    respond(make_json("read_all", "ok", "["+data+"]"));
  } else {
    respond(make_json("read_all", "error"));
  }
}

void read_pid(BYTE pid) {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: read_pid(0x"+String(pid, HEX)+")");
  String value = elm.get_pid_data(pid);
  if (!value.startsWith(elm.ERROR)) {
    respond(make_json("read_pid", "ok",  "["+make_jsondata_pid(pid,value)+"]"));
  } else {
    respond(make_json("read_pid", "error"));
  }
}

void read_car() {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: read_car()");
  boolean error = false;
  String data = "";
  while (true) {
    String value_vin = elm.get_vin();
    if (!value_vin.startsWith(elm.ERROR)) {
      data += "\"vin\":\""+value_vin+"\",";
    } else {error=true;break;}    
    String value_ecu = elm.get_ecu();
    if ((!error)&&(!value_ecu.startsWith(elm.ERROR))) {
      data += "\"ecu\":\""+value_ecu+"\",";
    } else {error=true;break;}     
    String value_voltage = elm.get_voltage();
    if ((!error)&&(!value_voltage.startsWith(elm.ERROR))) {
      data += "\"voltage\":\""+value_voltage+"\",";
    } else {error=true;break;}  
    String value_protocol = elm.get_protocol();
    if ((!error)&&(!value_protocol.startsWith(elm.ERROR))) {
      data += "\"protocol\":\""+value_protocol+"\"";
    } else {error=true;break;}     
    break;
  }
  
  if (!error) {
    respond(make_json("read_car","ok", "{"+data+"}"));
  } else {
    respond(make_json("read_car","error"));
  }
}

void read_dtc() {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: read_dct()");
  String data = elm.get_dtc();  
  if (!data.startsWith(elm.ERROR)) {
     //for parsed dtc's
    data.replace(",", "\",\"");
    respond(make_json("read_dtc","ok", "[\""+data+"\"]"));
    respond(make_json("read_dtc","ok","\""+data+"\""));
  } else {
    respond(make_json("read_dtc","error"));
  }
}

void delete_dtc() {
  //if (debugging) usb_serial.println("[Debugging] linuino cmd: delete_dct()");
  if (elm.clear_dtc()) {
    respond(make_json("delete_dtc","ok"));
  } else {
    respond(make_json("delete_dtc","error"));
  };
}



