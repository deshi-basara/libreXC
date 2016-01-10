/*
  elm.h - Library for communicating with ELM327 Shield.
  Created by Florian Beck, Viktor Pavlovic, Simon Schuster.
  Hochschule der Medien, Stuttgart.
  Stuttgart Media University
*/

#ifndef ELM_h
#define ELM_h

#include <Arduino.h>
#include <SoftwareSerial.h>

class ELM {
  private:
    byte _RX;
    byte _TX;
    int _UARTBAUD;
    SoftwareSerial *UART;
	boolean supported_pids[256];
	void get_available_pid_set(byte set);
  public:
    ELM(byte RX, byte TX);
    void begin(int UARTBAUD);
	String AT(String Cmd);
	String pid(byte id);
	void get_available_pids();
	
};

#endif