/*
  elm.h - Library for communicating with an ELM327 Shield.
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
    String AT(String Cmd);
    String pid(byte id);
    void update_available_pidset(byte set);
    void update_available_pids();    
    boolean available_pids[256];
    boolean available_pids_checked = false;
    void parsePID(byte pid, String raw_data, String *value, String *unit, String *desc, byte data_length);
    byte res2byte(String in, byte pos); // deprecated, use hex2byte instead
    byte hex2byte(String in, byte pos, String separator = "");
  public:
    ELM(byte RX, byte TX);
    void begin(int UARTBAUD);
    static const String ERROR;
    boolean reset();
    String get_available_pids();
    boolean pid_available(byte pid);
    String get_pid_rawdata(byte pid);
    String get_pid_data(byte pid);
    String get_pid_unit(byte pid);
    String get_pid_desc(byte pid);
    String get_vin();
    String get_ecu();
    String get_voltage();
    String get_protocol();
    String get_dtc();
    boolean clear_dtc();
};

#endif