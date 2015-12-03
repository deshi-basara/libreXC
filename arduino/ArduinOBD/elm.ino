/*
 * OBD-II-UART Code
 *
 * getResponse(), elm_get_speed() and elm_get_rpm() were taken from OBD-II-UART Quickstart Code by Ryan Owens for SparkFun Electronics
 *
 * This part of the code handles all communication with the OBD-II-UART module.
 * 
 */

void elm_start_com(){

  //Reset
  elm.println("ATZ");
  
  //Wait for OBD2UART to boot
  delay(2000);
  
  //Delete interferring data from serial port
  elm.flush();
}

// Query available PIDs
void elm_check_PIDs() {
  
  elm.println("0100");
  
  String raw_pids = rxData;
  raw_pids.trim();
  
  // TODO: Save available PIDs in a global pid list
}


// Query arbitrary PID
void elm_get_PID(char PID) {

 String sPID = String(PID, HEX);
 
 elm.println("01" + sPID);
 
  // First response = echo of command
  getResponse();
  
  // Second response = actual response data
  getResponse();  
  
  // TODO: Implement further response handling of arbitrary response data
}


// Query Speed
int elm_get_speed() {

  // 01 = get pid; 0D = speed
  elm.println("010D");
  
  // First response = echo of command
  getResponse();
  
  // Second response = actual response data
  getResponse();  
  
  /* Response format: 41 0D 20   (with spaces)
   *
   * Byte0+1 = 41 = Requested Mode + 0x40
   * Byte3+4 = 0D = Requested PID
   * Byte6+  = 20 = Requested Data
   *
   */
 
  
  return strtol(&rxData[6],0,16);
  
}

// Query RPM
int elm_get_rpm() {
  
   // 01 = get pud; 0C = rpm
  elm.println("010C");
  
  // First response = echo of command
  getResponse();
  
  // Second response = actual response data
  getResponse();
  
  //NOTE: RPM data is two bytes long, and delivered in 1/4 RPM from the OBD-II-UART
  int tmp_rpm = ((strtol(&rxData[6],0,16)*256)+strtol(&rxData[9],0,16))/4;
  return tmp_rpm;
} 


// Query Coolant Temp
int elm_get_coolant_temp() {
    
   // 01 = get pud; 05 = water temp
   elm.println("0105");
  
  // First response = echo of command
  getResponse();
  
  // Second response = actual response data
  getResponse();
  
  return strtol(&rxData[6],0,16);

}

void elm_get_voltage() {
  
  elm.println("ATRV");
  
  getResponse();
  getResponse();
  
  String raw_voltage1 = rxData;
  raw_voltage1.trim();
  String voltage = raw_voltage1.substring(0, (raw_voltage1.length()-2));
  
  vehicleVoltage = voltage.toFloat();
}

//The getResponse function collects incoming data from the UART into the rxData buffer
// and only exits when a carriage return character is seen. Once the carriage return
// string is detected, the rxData buffer is null terminated (so we can treat it as a string)
// and the rxData index is reset to 0 so that the next string can be copied.
void getResponse(void){
  
  char inChar=0;
  //Keep reading characters until we get a carriage return
  
  while(inChar != '\r'){
    //If a character comes in on the serial port, we need to act on it.
    if(elm.available() > 0){
      //Start by checking if we've received the end of message character ('\r').
      if(elm.peek() == '\r'){
        //Clear the Serial buffer
        inChar=elm.read();
        //Put the end of string character on our data string
        rxData[rx_i]='\0';
        //Reset the buffer index so that the next character goes back at the beginning of the string.
        rx_i=0;
      }
      //If we didn't get the end of message character, just add the new character to the string.
      else{
        //Get the new character from the Serial port.
        inChar = elm.read();
        //Add the new character to the string, and increment the index variable.
        rxData[rx_i++]=inChar;
      }
    }
  }
}
