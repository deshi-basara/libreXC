/*
 * Main Code block
 *
 *
 */
#include <SoftwareSerial.h>


// Important:
// Software Serial RX only possible on the following pins: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
SoftwareSerial elm(9,10);

// char buffer for serial Data
char rxData[20];
char rx_i=0;


//
boolean supported_pids[128];

//Variables to hold the speed and RPM data.
int vehicleSpeed = 0; ///DEBUG
int vehicleRPM = 0; ///DEBUG
int vehicleCoolantTemp = 0; ///DEBUG
float vehicleVoltage = 0; ///DEBUG

void setup() {
  
  pinMode(13, OUTPUT);
  
  elm.begin(9600);
  Serial.begin(9600);
  
  delay(50); // Wait for Serial readiness
  Serial.println(F("OBDuino Debug Console")); ///DEBUG Remove later
  
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

  elm_check_PIDs();
  delay(100);
  vehicleSpeed = elm_get_speed();
  delay(100);
  vehicleRPM = elm_get_rpm();
  delay(100);
  vehicleCoolantTemp = elm_get_coolant_temp();
  delay(100);
  elm_get_voltage();
  delay(100);
 
  Serial.write("Data: ");
  Serial.print("Speed: ");
  Serial.print(vehicleSpeed);
  Serial.print(" RPM: ");
  Serial.print(vehicleRPM);
  Serial.print(" Temp: ");
  Serial.println(vehicleCoolantTemp);
  
  delay(1300);
  // Test Code Viktor end : FEEL FREE TO REMOVE
}
