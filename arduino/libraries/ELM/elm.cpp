/*
  elm.cpp - Library for communicating with ELM327 Shield.
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

void ELM::begin(int UARTBAUD){
  _UARTBAUD = UARTBAUD;
  UART->begin(_UARTBAUD);
}


/*
 * Build AT-Command to get current data by specified PID
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
	
	//
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

	
	// fill supported pid list
	for (int i = 1; i < (bin1.length() + 1); i++) {

		if (bin1.charAt(i) == '0') {
			supported_pids[i] = false;
		} else {
			supported_pids[i] = true;
		}		
		
	}
	
	Serial.println(part1);
}	

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

#endif