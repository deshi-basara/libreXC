/*
 * Main Code block
 *
 *
 */
#include <SoftwareSerial.h>


SoftwareSerial elm(2,3);

// char buffer for serial Data
char rxData[20];
char rx_i=0;



//Variables to hold the speed and RPM data.
int vehicleSpeed = 0;
int vehicleRPM = 0;
int vehicleCoolantTemp = 0;
float vehicleVoltage = 0;

void setup() {
  
  pinMode(13, OUTPUT);
  
  elm.begin(9600);
  Serial.begin(9600);
  
  delay(1500);
  elm_start_com();
  delay(500);
}

void loop() {
  

  // Test Code Viktor begin : FEEL FREE TO REMOVE

  digitalWrite(13,HIGH);
  delay(100);
  digitalWrite(13,LOW);
  delay(100);

  vehicleSpeed = elm_get_speed();
  delay(100);
  vehicleRPM = elm_get_rpm();
  delay(100);
  vehicleCoolantTemp = elm_get_coolant_temp();
  delay(100);
  elm_get_voltage();
  delay(100);
 
  //Serial.write("Hello Car!");
  // Test Code Viktor end : FEEL FREE TO REMOVE
}
