#include <SoftwareSerial.h>

boolean debugging = true; //debugging global on/off
SoftwareSerial elm(9,10);

void setup() {
  Serial.begin(9600); //(Debugging) Serial USB
  while (!Serial) {;} // wait for serial port to connect. Needed for native USB port only
  elm.begin(9600); //Serial ELM  
}

void loop() {
  //this is just a test code which asks for temperature every second
  AT(pid(0x04));
  delay(1000);
}

/**
 * pid converts the PID id to the AT command.
 * 
 * @param byte id: PID id
 * @return String ret: response of the ELM327 Shield
 */
String pid(byte id) {
  String ret = "01"; //obd mode 1(current data)
  ret += (id<0x10)?"0":""; //leading zero
  ret += String(id,HEX); //pid
  ret.toUpperCase();
  if(debugging){Serial.println("[Debugging]{pid} AT Command for PID "+String(id)+": "+ret);}
  return ret;
}

/** 
 * AT handles the communication with the ELM327 Shield.
 *  
 * @param String Cmd: AT Command
 * @return String Response: response of the ELM327 Shield
 */
String AT(String Cmd) {  
  //generate command char array
  int len = Cmd.length();
  char cmd[len+1];
  Cmd.toCharArray(cmd,len+1);
  //send to elm
  elm.println(Cmd);
  if(debugging){Serial.println("[Debugging]{AT} AT Command: "+Cmd);}
  //wait for response and process received data
  int i = 0; //received characters  
  unsigned long timestamp = millis(); //set timestamp for timeout
  String Response;
  while(true) { //listen on serial port until response is complete
    if (elm.available()) {
      char response;
      if (i<len) { //echo not complete
        response = elm.read();i++;
        if (cmd[i-1]!=response){ //check echo characters
          if(debugging){Serial.println("[Debugging][ERROR]{AT} echo not complete");}
          //Todo: error handling
          break;
        }
      } else { //echo complete
        response = elm.read();i++;
        if (response!=62) {
          if (response!=13/*&&received!=32*/) {
            Response += String(response);
          }
        } else {
          if(debugging){Serial.println("[Debugging]{AT} Complete. Response string: "+Response);}
          return Response;
          break;
        }
      }
    }
    //timeout
    if (millis()>timestamp+2000) {
      if(debugging){Serial.println("[Debugging]{AT} Timeout");}
      break;
    }
  }
  if(debugging){Serial.println("[Debugging]{AT} Response time: "+String(millis()-timestamp))+" ms";}
}
