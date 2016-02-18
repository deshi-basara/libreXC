/*
  elm.cpp - Library for communicating with an ELM327 Shield.
  Created by Florian Beck, Viktor Pavlovic, Simon Schuster.
  Hochschule der Medien, Stuttgart.
  Stuttgart Media University
*/

#ifndef ELM_cpp
#define ELM_cpp

#include <Arduino.h>
#include "elm.h"

ELM::ELM(byte RX, byte TX){
  _RX = RX;
  _TX = TX;
  UART = new SoftwareSerial(_RX, _TX);
}

/*
 * Initialize serial communication with ELM chip
 *
 */
void ELM::begin(int UARTBAUD){
  _UARTBAUD = UARTBAUD;
  UART->begin(_UARTBAUD);
}


/*
 * Build AT command to get current data by specified PID
 *
 */
String ELM::pid(byte id) {
	String ret = "01"; //obd mode 1 (current data)
	ret += (id<0x10)?"0":""; //leading zero
	ret += String(id,HEX); //pid
	ret.toUpperCase();
	return ret;
}

/*
 * Get value by specified PID
 *
 */
String ELM::get_pid(byte id) {
	
	String data = AT(pid(id));
	
	if(!supported_pids[id]) {
		return "PID not supported by ECU";
	}	
	
	String myVal =  "";
	String myUnit = "";
	String myDesc = "";
	
	byte data_length = (data.length()  - 5 ) / 3;
	byte b_data[data_length];
	
	
	parsePID(id, data, &myVal, &myUnit, &myDesc, data_length);
	
	Serial.println(myVal + " " + myUnit + " (" + myDesc + ")" );
	
	return myVal;
}
 
 
/*
 * Convert ELM Mode 1 Command Response to Byte
 *
 */ 
byte ELM::res2byte(String in, byte pos) {
	
	// trim to continuous 32bit hex string
	String part1 = in.substring(6,8);
	part1 += in.substring(9,11);
	part1 += in.substring(12,14);
	part1 += in.substring(15,17);
	
	
	char p1char[part1.length() + 1];
	part1.toCharArray(p1char, part1.length() + 1);
	
	// convert to long
	unsigned long l1 = strtoul(p1char,NULL,16);
	
	unsigned char bytes[4];
	
	bytes[0] = (l1 >> 24) & 0xFF;
	bytes[1] = (l1 >> 16) & 0xFF;
	bytes[2] = (l1 >> 8) & 0xFF;
	bytes[3] = l1 & 0xFF;
	
	
	return bytes[pos];
}	
 
/*
 * Get complete list of supported PIDs
 *
 */
void ELM::get_available_pids_helper(){
	
	// initialize supported pid list
	for (int h = 0; h < 256; h++) {
		supported_pids[h] = false;
	}	
	supported_pids[0] = true; // PID0 is always supported and can't be checked for support
	
	
	get_available_pid_set(1);
	
	
	// Check if pid 0x20 is available (meaning next set is supported)
	if ( supported_pids[0x20] ) {
		
		get_available_pid_set(2);
	
		if ( supported_pids[0x40] ) {
		
			get_available_pid_set(3);
		
			if ( supported_pids[0x60] ) {
		
				get_available_pid_set(4);
				
				if ( supported_pids[0x80] ) {
		
					get_available_pid_set(5);
					
					if ( supported_pids[0xA0] ) {
		
						get_available_pid_set(6);
						
						if ( supported_pids[0xC0] ) {
		
							get_available_pid_set(7);
							
						}
					}
				}
			}	
		}	
	}	
	
	
}

/*
 * Get supported PIDs defined by set (1-7)
 *
 */
void ELM::get_available_pid_set(byte set) {
	
	String cmd1;
	
	// Select command
	switch (set) {
			case 1:
				cmd1 = "0100";
				break;
			case 2:
				cmd1 = "0120";
				break;
			case 3:
				cmd1 = "0140";
				break;
			case 4:
				cmd1 = "0160";
				break;
			case 5:
				cmd1 = "0180";
				break;
			case 6:
				cmd1 = "01A0";
				break;
			case 7:
				cmd1 = "01C0";
				break;	
			default:
				cmd1 = "0100";
				break;
	}	
	
	// Get first set of pids
	String seq1 = AT(cmd1);
	
	
	// trim to continuous 32bit hex string
	String part1 = seq1.substring(6,8);
	part1 += seq1.substring(9,11);
	part1 += seq1.substring(12,14);
	part1 += seq1.substring(15,17);
	
	char p1char[part1.length() + 1];
	part1.toCharArray(p1char, part1.length() + 1);
	
	// convert to long
	unsigned long l1 = strtoul(p1char,NULL,16);
	//convert to binary string
	String bin1 = String(l1, BIN);

	int m = (set-1) * 32;
	
	// fill supported pid list
	for (int i = 0; i < (bin1.length() + 1); i++) {

		if (bin1.charAt(i) == '0') {
			supported_pids[i+m] = false;
		} else {
			supported_pids[i+m] = true;
			Serial.print(String(i+m,DEC) + " ");//DEBUG
		}		
		
	}
	
}	

/*
 * reset ELM chip
 *
 */
void ELM::reset() {
	
	AT("ATZ");
}	

/*
 * Clear stored trouble codes 
 * 
 */
void ELM::clearDTC() {

	AT("04");
}

/*
 * Read stored trouble codes
 *  
 */
void ELM::readDTC() {
	
	Serial.println(AT("03"));
}

/*
 * ELM327 AT command and response handler
 *
 */
String ELM::AT(String Cmd)
{
  //generate command char array
  int len = Cmd.length();
  char cmd[len+1];
  Cmd.toCharArray(cmd,len+1);
  //send to elm
  UART->println(Cmd);
  //wait for response and process received data
  int i = 0; //received characters  
  unsigned long timestamp = millis(); //set timestamp for timeout
  String Response;
  while(true) { //listen on serial port until response is complete
    if (UART->available()) {
      char response;
      if (i<len) { //echo not complete
        response = UART->read();
        i++;
        if (cmd[i-1]!=response){ //check echo characters
          //Todo: error handling
          break;
        }
      } 
      else { //echo complete
        response = UART->read();
        i++;
        if (response!=62) {
          if (response!=13/*&&received!=32*/) {
            Response += String(response);
          }
        } 
        else {
          return Response;
          break;
        }
      }
    }
    //timeout
    if ((unsigned long)(millis()-timestamp)>2000) {
      break;
    }
  }
}


/*
 * Return parsed value, unit and PID description from supported raw data
 *
 */
void ELM::parsePID(byte pid, String raw_data, String *value, String *unit, String *desc, byte data_length) {
	
	byte data[13];
	
	for(int n = 0; n < data_length; n++){
		data[n] = res2byte(raw_data, n);	
	}
	
	#define A data[0]
	#define B data[1]
	#define C data[2]
	#define D data[3]
	#define E data[4]
	
	#define CALC_PERCENT \
		*value=String(data[0]*100.0/255.0); \
		*unit="%";
		
	#define CALC_TEMP \
		*value=String(data[0]-40.0); \
		*unit="°C";
		
	// Helper Variables
	String value_three_tmp;
	String temp1C;	
	
  switch (pid) {
	case 0x03: // Fuel system status (BIT ENCODED)
		
		switch(A) { // Fuel system #1
			case 1:
				value_three_tmp=String(F("Open loop due to insufficient engine temperature"));
				break;
			case 2:
				value_three_tmp=String(F("Closed loop, using oxygen sensor feedback to determine fuel mix"));
				break;
			case 4:
				value_three_tmp=String(F("Open loop due to engine load OR fuel cut due to deceleration"));
				break;
			case 8:
				value_three_tmp=String(F("Open loop due to system failure"));
				break;
			case 16:
				value_three_tmp=String(F("Closed loop, using at least one oxygen sensor but there is a fault in the feedback system"));
				break;
			default:
				value_three_tmp=String(F("ERROR - INVALID VALUE"));
				break;
		}
		switch (B) { // Fuel system #2 (if present)
			case 0:
				break;
			case 1:
				value_three_tmp = "Fuel System 1: " + value_three_tmp + String(F(", Fuel System 2: Open loop due to insufficient engine temperature"));
				break;
			case 2:
				value_three_tmp = "Fuel System 1: " + value_three_tmp + String(F(", Fuel System 2: Closed loop, using oxygen sensor feedback to determine fuel mix"));
				break;
			case 4:
				value_three_tmp = "Fuel System 1: " + value_three_tmp + String(F(", Fuel System 2: Open loop due to engine load OR fuel cut due to deceleration"));
				break;
			case 8:
				value_three_tmp = "Fuel System 1: " + value_three_tmp + String(F(", Fuel System 2: Open loop due to system failure"));
				break;
			case 16:
				value_three_tmp = "Fuel System 1: " + value_three_tmp + String(F(", Fuel System 2: Closed loop, using at least one oxygen sensor but there is a fault in the feedback system"));
				break;
			default:
				value_three_tmp = "Fuel System 1: " + value_three_tmp + String(F(", Fuel System 2: ERROR - INVALID VALUE"));
				break;	
		}	
		*value = value_three_tmp;
		*unit = "";
		*desc = F("Fuel system status");
		break;
    case 0x04: //Calculated engine load value
      CALC_PERCENT
      *desc=F("Calculated engine load value");
      break;
    case 0x05: //Engine coolant temperature
      CALC_TEMP
      *desc=F("Engine coolant temperature");
      break;
	#define CALC_STFT \
      *value=String((data[0]-128.0)*100.0/128.0); \
      *unit="%";
    case 0x06:
		CALC_STFT
		*desc=F("Long term fuel % trim - Bank 1");
		break;
    case 0x07:
		CALC_STFT
		*desc=F("Long term fuel % trim - Bank 1");
		break;
    case 0x08:
		CALC_STFT
		*desc=F("Long term fuel % trim - Bank 2");
		break;
    case 0x09:
		CALC_STFT
		*desc=F("Long term fuel % trim - Bank 2");
		break;
    case 0x0A:
		*value=String(data[0]*3);
		*unit="kPa";
		*desc=F("Fuel pressure");
		break;
    case 0x0B:
		*value=String(data[0]);
		*unit="kPa";
		*desc=F("Intake manifold absolute pressure");
		break;
	case 0x0C:
		*value=String(((data[0]*256.0)+data[1])/4.0);
		*unit="rpm";
		*desc=F("Engine RPM");
		break;
	case 0x0D:
	  *value=String(data[0]);
	  *unit="km/h";
	  *desc=F("Vehicle speed");
	  break;
	case 0x0E:
	  *value=String((data[0]-128.0)/2.0);
	  *unit="deg";
	  *desc=F("Timing advance");
	  break;
	case 0x0F:
	  CALC_TEMP
	  *desc=F("Intake air temperature");
	  break;
	case 0x10:
	  *value=String( ((data[0]*256.0) + data[1])/100.0 );
	  *unit="g/s";
	  *desc=F("MAF air flow rate");
	  break;
	case 0x11:
	  CALC_PERCENT
	  *desc=F("Throtle position");
	  break;
	case 0x12:
		switch(data[0]) {
			case 1:
				*value=String(F("Upstream"));
				break;
			case 2:
				*value=String(F("Downstream of catalytic converter"));
				break;
			case 4:
				*value=String(F("From the outside atmosphere or off"));
				break;
			case 8:
				*value=String(F("Pump commanded on for diagnostics"));
				break;
			default:
				*value=String(F("ERROR - INVALID VALUE"));
				break;
		}
	  
	  *unit="";
	  *desc=F("Commanded secondary air status");
	  break;
	case 0x13:
		*value=String(data[0]); //BIT ENCODED TODO: DECODE
		*unit="";
		*desc=F("Oxygen sensors present");
	  break;
	 #define CALC_OS_VAL \
	  *value=String( (data[0] / 200.0)  ) + "," + String( (data[1] - 128.0) * 100.0 / 128.0 ); \
	  *unit="V, %";
	case 0x14:
		*desc=F("Bank 1, Sensor1: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;
	case 0x15:
		*desc=F("Bank 1, Sensor2: Oxygen sensor voltage, Short term fuel trim");	  
		CALC_OS_VAL
		break;
	case 0x16:
		*desc=F("Bank 1, Sensor3: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;
	case 0x17:
		*desc=F("Bank 1, Sensor4: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;
	case 0x18:
		*desc=F("Bank 2, Sensor1: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;
	case 0x19:
		*desc=F("Bank 2, Sensor2: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;
	case 0x1A:
		*desc=F("Bank 2, Sensor3: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;
	case 0x1B:
		*desc=F("Bank 2, Sensor4: Oxygen sensor voltage, Short term fuel trim");
		CALC_OS_VAL
		break;

	case 0x1C:
	
		
		switch(A) {
		
			case 1:	temp1C = F("OBD-II as defined by the CARB"); break;
			case 2:	temp1C = F("OBD as defined by the EPA"); break;
			case 3:	temp1C = F("OBD and OBD-II"); break;
			case 4:	temp1C = F("OBD-I"); break;
			case 5:	temp1C = F("Not OBD compliant"); break;
			case 6:	temp1C = F("EOBD (Europe)"); break;
			case 7:	temp1C = F("EOBD and OBD-II"); break;
			case 8:	temp1C = F("EOBD and OBD"); break;
			case 9:	temp1C = F("EOBD, OBD and OBD II"); break;
			case 10:	temp1C = F("JOBD (Japan)"); break;
			case 11:	temp1C = F("JOBD and OBD II"); break;
			case 12:	temp1C = F("JOBD and EOBD"); break;
			case 13:	temp1C = F("JOBD, EOBD, and OBD II"); break;

			case 17:	temp1C = F("Engine Manufacturer Diagnostics (EMD)"); break;
			case 18:	temp1C = F("Engine Manufacturer Diagnostics Enhanced (EMD+)"); break;
			case 19:	temp1C = F("Heavy Duty On-Board Diagnostics (Child/Partial) (HD OBD-C)"); break;
			case 20:	temp1C = F("Heavy Duty On-Board Diagnostics (HD OBD)"); break;
			case 21:	temp1C = F("World Wide Harmonized OBD (WWH OBD)"); break;

			case 23:	temp1C = F("Heavy Duty Euro OBD Stage I without NOx control (HD EOBD-I)"); break;
			case 24:	temp1C = F("Heavy Duty Euro OBD Stage I with NOx control (HD EOBD-I N)"); break;
			case 25:	temp1C = F("Heavy Duty Euro OBD Stage II without NOx control (HD EOBD-II)"); break;
			case 26:	temp1C = F("Heavy Duty Euro OBD Stage II with NOx control (HD EOBD-II N)"); break;
			case 28:	temp1C = F("Brazil OBD Phase 1 (OBDBr-1)"); break;
			case 29:	temp1C = F("Brazil OBD Phase 2 (OBDBr-2)"); break;
			case 30:	temp1C = F("Korean OBD (KOBD)"); break;
			case 31:	temp1C = F("India OBD I (IOBD I)"); break;
			case 32:	temp1C = F("India OBD II (IOBD II)"); break;
			case 33:	temp1C = F("Heavy Duty Euro OBD Stage VI (HD EOBD-IV)"); break;	
			case 251:	case 252:	case 253:	case 254:	case 255:
				temp1C = F("Not available for assignment (SAE J1939 special meaning)"); break;
				break;
			default:
				temp1C = F("Reserved"); break;
				break;
		}
	
		*value=temp1C;
		*unit="";
		*desc=F("OBD standards this vehicle conforms to");
		break;
	case 0x1D:
		*value=String(data[0]); // BIT ENCODED TODO: DECODE
		*unit="";
		*desc=F("Oxygen sensors present");
		break;
	case 0x1E:
	  if(data[0] == 1) {
		*value=String(F("PTO active"));
	  } else {
		*value=String(F("PTO inactive"));
	  }
	  *unit="";
	  *desc=F("Auxillary input status");
	  break;  
	case 0x1F:
	  *value=String((data[0]*256) + data[1]);
	  *unit="s";
	  *desc=F("Run time since engine start");
	  break;  
	case 0x21:
	  *value=String((data[0]*256) + data[1]);
	  *unit="km";
	  *desc=F("Distance traveled with MIL on");
	  break;  
	case 0x22:
	  *value=String(( (data[0]*256.0) + data[1] ) * 0.079);
	  *unit="kPa";
	  *desc=F("Fuel rail pressure relative to manifold vacuum");
	  break;  
	case 0x23:
	  *value=String( ( (data[0]*256) + data[1] ) * 10);
	  *unit="kPa";
	  *desc=F("Fuel rail pressure");
	  break;  
	#define CALC_O2S \
	  *value = String ( ((data[0]*256.0) + data[1]) * 2.0 / 65535.0 ) + ", " + String( ((data[2]*256.0) + data[3]) * 8.0 / 65535.0 ) ; \
	  *unit = "n/a , V"; 
	case 0x24:
		CALC_O2S
		*desc=F("O2S1_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x25:
		CALC_O2S
		*desc=F("O2S2_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x26:
		CALC_O2S
		*desc=F("O2S3_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x27:
		CALC_O2S
		*desc=F("O2S4_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x28:
		CALC_O2S
		*desc=F("O2S5_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x29:
		CALC_O2S
		*desc=F("O2S6_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x2A:
		CALC_O2S
		*desc=F("O2S7_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	case 0x2B:
		CALC_O2S
		*desc=F("O2S8_WR_lambda(1): Equivalence ratio, Voltage");
		break;
	
	case 0x2C:
	  *value=String(data[0]*100.0 / 255.0);
	  *unit="%";
	  *desc=F("Commanded EGR");
	  break;  
	case 0x2D:
	  *value=String((data[0]-128.0) * 100.0 / 255.0);
	  *unit="%";
	  *desc=F("EGR Error");
	  break;
	case 0x2E:
	  *value=String(data[0] * 100.0 / 255.0);
	  *unit="%";
	  *desc=F("Commanded evaporative purge");
	  break;  
	case 0x2F:
	  *value=String(data[0] * 100.0 / 255.0);
	  *unit="%";
	  *desc=F("Fuel level input");
	  break;    	
	case 0x30:
		*value=String(data[0]);
		*unit="";
		*desc=F("");
		break;
	case 0x31:
		*value=String(data[0]);
		*unit="";
		*desc=F("");
		break;
	case 0x32: // TWOs Complement
		*value=String(data[0]);
		*unit="";
		*desc=F("");
		break;
	case 0x33:
		*value=String(data[0]);
		*unit="";
		*desc=F("");
		break;
	#define CALC_O2SB \
	  *value = String ( ((data[0]*256.0) + data[1]) * 2.0 / 65535.0 ) + ", " + String( (((data[2]*256.0) + data[3]) / 256.0) - 128.0 ) ; \
	  *unit = "n/a , mA"; 	
	case 0x34:
		CALC_O2SB
		*desc=F("O2S1_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x35:
		CALC_O2SB
		*desc=F("O2S2_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x36:
		CALC_O2SB
		*desc=F("O2S3_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x37:
		CALC_O2SB
		*desc=F("O2S4_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x38:
		CALC_O2SB
		*desc=F("O2S5_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x39:
		CALC_O2SB
		*desc=F("O2S6_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x3A:
		CALC_O2SB
		*desc=F("O2S7_WR_lambda(1): Equivalence ratio, Current");
		break;
	case 0x3B:
		CALC_O2SB
		*desc=F("O2S8_WR_lambda(1): Equivalence ratio, Current");
		break;
	#define CALC_CAT_TEMP \
	  *value = String ( (((data[0]*256)+data[1])/10) - 40 ); \
	  *unit = "°C"; 		
	case 0x3C:
		CALC_CAT_TEMP
		*desc=F("Catalyst Temperature Bank 1, Sensor 1");
		break;
	case 0x3D:
		CALC_CAT_TEMP
		*desc=F("Catalyst Temperature Bank 2, Sensor 1");
		break;
	case 0x3E:
		CALC_CAT_TEMP
		*desc=F("Catalyst Temperature Bank 1, Sensor 2");
		break;
	case 0x3F:
		CALC_CAT_TEMP
		*desc=F("Catalyst Temperature Bank 2, Sensor 2");
		break;
	case 0x41:
		*value=String(data[0]); // BIT ENCODED
		*unit="";
		*desc=F("Monitor status this drive cycle");
		break;
	case 0x42:
		*value=String(((A*256.0)+B)/1000.0);
		*unit="V";
		*desc=F("Control module voltage");
		break;
	case 0x43:
		*value=String(((A*256.0)+B)*100.0/255.0);
		*unit="%";
		*desc=F("Absolute load value");
		break;
	case 0x44:
		*value=String(((A*256.0)+B)/32768.0);
		*unit="";
		*desc=F("Fuel/Air commanded equivalence ratio");
		break;
	case 0x45:
		CALC_PERCENT
		*desc=F("Relative throttle position");
		break;
	case 0x46:
		*value=String(A-40.0);
		*unit="°C";
		*desc=F("Ambient air temperature");
		break;
	case 0x47:
		CALC_PERCENT
		*desc=F("Absolute throttle position B");
		break;
	case 0x48:
		CALC_PERCENT
		*desc=F("Absolute throttle position C");
		break;
	case 0x49:
		CALC_PERCENT
		*desc=F("Accelerator pedal position D");
		break;
	case 0x4A:
		CALC_PERCENT
		*desc=F("Accelerator pedal position E");
		break;
	case 0x4B:
		CALC_PERCENT
		*desc=F("Accelerator pedal position F");
		break;
	case 0x4C:
		CALC_PERCENT
		*desc=F("Commanded throttle actuator");
		break;
	case 0x4D:
		*value=String((A*256.0)+B);
		*unit="min";
		*desc=F("Time run with MIL on");
		break;
	case 0x4E:
		*value=String((A*256.0)+B);
		*unit="min";
		*desc=F("Time since trouble codes cleared");
		break;
	case 0x4F:
		*value=String(A) + ", " + String(B) + ", " + String(C) + ", " + String(D*10.0);
		*unit="n/a, V, mA, kPa";
		*desc=F("Maximum value for equivalence ratio, oxygen sensor voltage, oxygen sensor current, and intake manifold absolute pressure");
		break;	
	case 0x50:
		*value=String(A*10) + ", " + String(B) + ", " + String(C) + ", " + String(D);
		*unit="g/s";
		*desc=F("Maximum value for air flow rate from mass air flow sensor");
		break;	
	case 0x51:
		*value=String(data[0]); // INT ENCODED
		*unit="";
		*desc=F("Fuel Type");
		break;	
	case 0x52:
		CALC_PERCENT
		*desc=F("Ethanol fuel %	");
		break;	
	case 0x53:
		*value=String(((A*256.0)+B)/200.0);
		*unit="kPa";
		*desc=F("Absolute Evap system Vapor Pressure");
		break;	
	case 0x54:
		*value=String(((A*256.0)+B)-32767.0);
		*unit="";
		*desc=F("Evap system vapor pressure	");
		break;	
		#define CALC_STSOS \
		*value=String((A-128.0)*100.0/128.0) + ", " + String(String((B-128.0)*100.0/128.0)); \
		*unit="%, %";
	case 0x55:
		CALC_STSOS
		*desc=F("Short term secondary oxygen sensor trim bank 1 and bank 3");
		break;	
	case 0x56:
		CALC_STSOS
		*desc=F("Long term secondary oxygen sensor trim bank 1 and bank 3");
		break;	
	case 0x57:
		CALC_STSOS
		*desc=F("Short term secondary oxygen sensor trim bank 2 and bank 4");
		break;	
	case 0x58:
		CALC_STSOS
		*desc=F("Long term secondary oxygen sensor trim bank 2 and bank 4");
		break;
	case 0x59:
		*value=String(((A*256.0)+B) * 10.0);
		*unit="kPa";
		*desc=F("Fuel rail pressure (absolute)");
		break;
	case 0x5A:
		CALC_PERCENT
		*desc=F("Relative accelerator pedal position");
		break;
	case 0x5B:
		CALC_PERCENT
		*desc=F("Hybrid battery pack remaining life	");
		break;
	case 0x5C:
		CALC_TEMP
		*desc=F("Engine oil temperature");
		break;
	case 0x5D:
		*value=String((((A*256.0)+B)-26880.0)/128.0);
		*unit="°";
		*desc=F("Fuel injection timing");
		break;
	case 0x5E:
		*value=String(((A*256.0)+B)*0.05);
		*unit="L/h";
		*desc=F("Engine fuel rate	");
		break;
	case 0x5F:
		*value=String(A); // bit encoded
		*unit="";
		*desc=F("Emission requirements to which vehicle is designed");
		break;
	#define CALC_MOD_PERCENT \
		*value=String(data[0] - 125); \
		*unit="%";
	case 0x61:
		CALC_MOD_PERCENT
		*desc=F("Driver's demand engine - percent torque");
		break;
	case 0x62:
		CALC_MOD_PERCENT
		*desc=F("Actual engine - percent torque");
		break;
	case 0x63:
		*value=String(A*256.0+B);
		*unit="";
		*desc=F("Engine reference torque");
		break;
	case 0x64:
		*value=String(A-125) + ", " + String(B-125) + ", " + String(C-125) + ", " + String(D-125) + ", " + String(E-125);
		*unit="%";
		*desc=F("Engine percent torque data - IDLE, Engine Point 1, EP2, EP3, EP4");
		break;	
	case 0x65:
		*value=String(); // BIT ENCODED
		*unit="";
		*desc=F("Auxiliary input / output supported	");
		break;	
	case 0x66:
		*value=String(A) + ", " + String(B) + ", " + String(C) + ", " + String(D) + ", " + String(E); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Mass air flow sensor");
		break;	
	case 0x67:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Engine coolant temperature");
		break;	
	case 0x68:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Intake air temperature sensor");
		break;	
	case 0x69:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Commanded EGR and EGR Error");
		break;	
	case 0x6A:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Commanded Diesel intake air flow control and relative intake air flow position");
		break;	
	case 0x6B:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Exhaust gas recirculation temperature");
		break;	
	case 0x6C:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Commanded throttle actuator control and relative throttle position");
		break;	
	case 0x6D:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Fuel pressure control system");
		break;	
	case 0x6E:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Injection pressure control system");
		break;	
	case 0x6F:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Turbocharger compressor inlet pressure");
		break;	
	case 0x70:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Boost pressure control");
		break;
	case 0x71:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Variable Geometry turbo (VGT) control");
		break;
	case 0x72:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Wastegate control");
		break;
	case 0x73:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Exhaust pressure");
		break;
	case 0x74:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Turbocharger RPM");
		break;
	case 0x75:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Turbocharger temperature");
		break;
	case 0x76:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Turbocharger temperature");
		break;
	case 0x77:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Charge air cooler temperature (CACT)");
		break;
	case 0x78:
		*value=String(); // special pid
		*unit="";
		*desc=F("Exhaust Gas temperature (EGT) Bank 1");
		break;
	case 0x79:
		*value=String(); // special pid
		*unit="";
		*desc=F("Exhaust Gas temperature (EGT) Bank 2");
		break;
	case 0x7A:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Diesel particulate filter (DPF)");
		break;
	case 0x7B:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Diesel particulate filter (DPF)");
		break;
	case 0x7C:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Diesel Particulate filter (DPF) temperature");
		break;
	case 0x7D:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("NOx NTE control area status");
		break;
	case 0x7E:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("PM NTE control area status	");
		break;
	case 0x7F:
		*value=String(); // NO FORMULA AVAILABLE YET
		*unit="";
		*desc=F("Engine run time");
		break;	
    // usw.
  }
}

#endif