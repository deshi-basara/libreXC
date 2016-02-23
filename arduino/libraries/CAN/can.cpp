/*
  can.cpp - Library for communicating with an CAN Shield.
  Created by Florian Beck, Viktor Pavlovic, Simon Schuster.
  Hochschule der Medien, Stuttgart.
  Stuttgart Media University
*/

#ifndef CAN_cpp
#define CAN_cpp

#include <Arduino.h>
#include "can.h"
#include <mcp_can.h>
#include <SPI.h>


CAN::CAN(int CS_pin = 9){
  MCP_CAN CAN0(CS_pin);
}

/*
 * Initialize SPI communication with CAN chip
 * TODO: variable baud rate
 */
boolean CAN::begin(){
  if (CAN0.begin(CAN_500KBPS) == CAN_OK) {
    // DEBUG: OK
	return true;
  } else {
    // DEBUG: ERROR
	return false;
    }
  }
}

const String CAN::ERROR = "ERROR";

void CAN::send_message(byte id, int dlc, byte[] data) {
	
	CAN0.sendMsgBuf(id, 0, dlc, data);
	
}

void CAN::send_message_multi(byte id, int dlc, byte[] data, int number, int delay_ms) {
	
	for (int i = 0; i < number; i++) {
		CAN0.sendMsgBuf(id, 0, dlc, data);
		delay(delay_ms);
	}	
	
}


#endif