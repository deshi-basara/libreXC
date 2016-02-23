/*
  elm.h - Library for communicating with an ELM327 Shield.
  Created by Florian Beck, Viktor Pavlovic, Simon Schuster.
  Hochschule der Medien, Stuttgart.
  Stuttgart Media University
*/

#ifndef CAN_h
#define CAN_h

#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>

class CAN {
  private:
   
  public:
    CAN(int CS_pin);
    boolean begin();
    static const String ERROR;
    send_message(byte id, int dlc, byte[] data);
	send_message_multi(byte id, int dlc, byte[] data, int number, int delay_ms);
};

#endif