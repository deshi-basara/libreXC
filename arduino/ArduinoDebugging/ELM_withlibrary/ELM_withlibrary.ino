#include <elm.h>

ELM elm(9,10);

void setup() {
  Serial.begin(9600); 
  elm.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(elm.AT("0105"));
  delay(2000);
}
