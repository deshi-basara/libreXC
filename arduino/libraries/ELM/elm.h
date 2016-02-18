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
    void get_available_pid_set(byte set);
    void get_available_pids_helper();    
    boolean available_pids[256];
    boolean available_pids_checked = false;
    void parsePID(byte pid, String raw_data, String *value, String *unit, String *desc, byte data_length);
    byte res2byte(String in, byte pos);
  public:
    ELM(byte RX, byte TX);
    void begin(int UARTBAUD);
    static const String ERROR;
    
    // reset
    boolean reset();
    // available_pids
    String get_available_pids(); /* gibt die verfügbaren PIDs als kommagetrennte Liste zurück,
                                    schreibt supported_pids */ 
    boolean pid_available(byte pid);
    // read_all
    // gibt's in der Library nicht, wird im Arduino-Sketch umgesetzt
    // read_pid
    String get_pid_rawdata(byte pid);
    String get_pid_data(byte pid);
    String get_pid_unit(byte pid);
    String get_pid_desc(byte pid);
    // read_car
    String get_vin();
    String get_ecu();
    String get_voltage();
    // read_dct
    String get_dtc();
    // delete_dct
    boolean clear_dtc();
};

#endif