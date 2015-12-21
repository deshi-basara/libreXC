#include <Bridge.h>
#include <SoftwareSerial.h>


boolean debugging = true; //debugging global on/off
SoftwareSerial elm(9,10);

void setup() {
  Serial.begin(9600); //(Debugging) Serial USB
  if (debugging) {while (!Serial) {;}}; // wait for serial monitor
  elm.begin(9600); //Serial ELM  
  Bridge.begin(); // make contact with linuino
  
  delay(500); // wait for elm to boot (in case of hard reset)
  elm.println("ATZ"); // reset elm
  delay(500); // wait for elm to boot
}

void loop() {
  //this is just a test code which asks for temperature every second
  getPID(0x0D);
  delay(1000);
}

/**
 * getPID
 * 
 * @param byte id: PID id
 * @return 
 */
void getPID(byte id) {
  String data = AT(pid(id));
  if(debugging){Serial.println("[Debugging]{getPID} received data: "+data);}
  // Daten testweise an Linuino schicken
  Bridge.put("Data", data);
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
  if(debugging){
    Serial.println("[Debugging]{pid} AT Command for PID "+String(id)+": "+ret);
  }
  return ret;
}

void get_available_pids() {
	
	String raw_a = AT(pid(0x00)); //retrieve PIDs 0x01 - 0x20
	
	//0  3  6  9  12 15
	//41 00 12 34 56 78
	
	raw_a.remove(0,6);
	//0  3  6  9  12 15
	//12 34 56 78
	
	raw_a.remove(2,1);
	//0  3  6  9  12 15
	//1234 56 78
	
	raw_a.remove(4,1);
	//0  3  6  9  12 15
	//123456 78
	
	raw_a.remove(6,1);
	
	String raw_a1 = raw_a.substring(0,4);
	String raw_a2 = raw_a.substring(4,4);
	
	unsigned int p1 = raw_a1.toInt();
	unsigned int p2 = raw_a2.toInt();
	
	String ba = String(p1, BIN);
	ba += String(p2, BIN);
	
	if(debugging) Serial.println("[Debugging]{get_available_pids} Received: " +  ba);
	
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
  if(debugging){
    Serial.println("[Debugging]{AT} AT Command: "+Cmd);
  }
  //wait for response and process received data
  int i = 0; //received characters  
  unsigned long timestamp = millis(); //set timestamp for timeout
  String Response;
  while(true) { //listen on serial port until response is complete
    if (elm.available()) {
      char response;
      if (i<len) { //echo not complete
        response = elm.read();
        i++;
        if (cmd[i-1]!=response){ //check echo characters
          if(debugging){
            Serial.println("[Debugging][ERROR]{AT} echo not complete");
          }
          //Todo: error handling
          break;
        }
      } 
      else { //echo complete
        response = elm.read();
        i++;
        if (response!=62) {
          if (response!=13/*&&received!=32*/) {
            Response += String(response);
          }
        } 
        else {
          if(debugging){
            Serial.println("[Debugging]{AT} Complete. Response string: "+Response);
          }
          return Response;
          break;
        }
      }
    }
    //timeout
    if (millis()>timestamp+2000) {
      if(debugging){
        Serial.println("[Debugging]{AT} Timeout");
      }
      break;
    }
  }
  if(debugging){
    Serial.println("[Debugging]{AT} Response time: "+String(millis()-timestamp))+" ms";
  }
}

