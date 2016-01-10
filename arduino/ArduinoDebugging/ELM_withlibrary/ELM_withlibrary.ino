#include <SoftwareSerial.h>

#include <elm.h>

ELM elm(9,10);

void setup() {
  Serial.begin(9600); 
  elm.begin(9600); 
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(elm.AT("0100")); // 41 00 be 3e a8 11
  elm.get_available_pids();
  delay(2000);
}
