#include <SoftwareSerial.h>

SoftwareSerial elm(9,10);
boolean debugging = true; //debugging global on/off

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  elm.begin(9600);
  
}

void loop() {
  AT(pid(0x04));
}

String pid(byte id) {
  String ret = "01"; //obd mode 1(current data)
  ret += (id<0x10)?"0":""; //leading zero
  ret += String(id,HEX); //pid
  ret.toUpperCase();
  if(debugging){Serial.println("[Debugging][pid] AT Command for PID "+String(id)+": "+ret);}
  return ret;
}

void AT(String Cmd) {
  //generate command string
  
  //generate command char array
  int len = Cmd.length();
  char cmd[len+1];
  Cmd.toCharArray(cmd,len+1);
  //send to elm
  elm.println(Cmd);
  if(debugging){Serial.println("[Debugging][AT] AT Command: "+Cmd);}

  int i = 0; //received characters  
  unsigned long timestamp = millis(); //set timestamp for timeout
  String response;
  while(true) {
    if (elm.available()) {
      int rec;
      if (i<len) { //echo not complete
        rec = elm.read();i++;
        if (cmd[i-1]!=rec){ //check echo characters
          if(debugging){Serial.println("[Debugging][AT] Zeichen "+String(i-1)+" nicht identisch ("+String(rec)+"). Abbruch.");}
          //Todo: error handling
          break;
        }
      } else { //echo complete
        rec = elm.read();i++;
        if (rec!=62) {
          if (rec!=13&&rec!=32) {
            response += String(rec);
          }
        } else {
          if(debugging){Serial.println("[Debugging][AT] Response String: "+response);}
          break;
        }
      }
    }
    //timeout
    if (millis()>timestamp+2000) {
      if(debugging){Serial.println("[Debugging][AT] Timeout");}
      break;
    }
  }
  if(debugging){Serial.println("[Debugging][AT] Time for this query: "+String(millis()-timestamp))+"ms";}
}
