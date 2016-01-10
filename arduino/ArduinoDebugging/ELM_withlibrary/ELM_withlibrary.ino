#include <Bridge.h>
#include <SoftwareSerial.h>
#include <elm.h>

boolean debugging = true; //debugging global on/off
ELM elm(9,10);

void setup() {
  Serial.begin(9600); //(Debugging) Serial USB
  if (debugging) {while (!Serial) {;}}; // wait for serial monitor
  
  elm.begin(9600); //Serial ELM  
  Bridge.begin(); // make contact with linuino
  
  delay(1000);
  Serial.println("Hello Car!"); //DEBUG
  
  elm.get_available_pids();
  Serial.println(""); //DEBUG
}

void loop() {
  
  
  delay(1000);
  elm.get_pid(0x05);
  
  
  
}

void all_data() {
	
	// list of supported_pids
	
	// conversion
	
	//Bridge.put("Data", data);
}	