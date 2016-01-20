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
void ELM::get_available_pids(){
	
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
    if (millis()>timestamp+2000) {
      break;
    }
  }
}


/*
 * Return parsed value, unit and PID description from supported raw data
 *
 */
void ELM::parsePID(byte pid, String raw_data, String *value, String *unit, String *desc, byte data_length) {
	
	byte data[4];
	
	for(int n = 0; n < data_length; n++){
		data[n] = res2byte(raw_data, n);	
	}
	
  switch (pid) {
    case 0x04: //Calculated engine load value
      *value=String(data[0]*100/255);
      *unit="%";
      *desc="Calculated engine load value";
      break;
    case 0x05: //Engine coolant temperature
      *value=String(data[0]-40);
      *unit="°C";
      *desc="Engine coolant temperature";
      break;
	#define CALC_STFT \
      *value=String((data[0]-128.0)*100.0/128.0); \
      *unit="%";
    case 0x06:
		CALC_STFT
		*desc="Long term fuel % trim - Bank 1";
		break;
    case 0x07:
		CALC_STFT
		*desc="Long term fuel % trim - Bank 1";
		break;
    case 0x08:
		CALC_STFT
		*desc="Long term fuel % trim - Bank 2";
		break;
    case 0x09:
		CALC_STFT
		*desc="Long term fuel % trim - Bank 2";
		break;
    case 0x0A:
		*value=String(data[0]*3);
		*unit="kPa";
		*desc="Fuel pressure";
		break;
    case 0x0B:
		*value=String(data[0]);
		*unit="kPa";
		*desc="Intake manifold absolute pressure";
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
	  *value=String(data[0]-40);
	  *unit="°C";
	  *desc=F("Intake air temperature");
	  break;
	case 0x10:
	  *value=String( ((data[0]*256.0) + data[1])/100.0 );
	  *unit="g/s";
	  *desc=F("MAF air flow rate");
	  break;
	case 0x11:
	  *value=String( data[0]*100.0 / 256.0 );
	  *unit="%";
	  *desc=F("Throtle position");
	  break;
	case 0x12:
	  *value=String(data[0]); //BIT ENCODED TODO: DECODE
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
		*value=String(data[0]); // BIT ENCODED TODO: DECODE
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
    // usw.
  }
}

#endif