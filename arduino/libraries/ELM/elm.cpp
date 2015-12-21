/*
  elm.cpp - Library for communicating with ELM327 Shield.
  Created by Florian Beck, Viktor Pavlovic, Simon Schuster.
  Hochschule der Medien, Stuttgart.
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

String ELM::pid(byte id) {
	String ret = "01"; //obd mode 1(current data)
	ret += (id<0x10)?"0":""; //leading zero
	ret += String(id,HEX); //pid
	ret.toUpperCase();
	return ret;
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