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
    void parsePID(byte pid, String raw_data, String *value, String *unit, String *desc, byte data_length);
    byte res2byte(String in, byte pos);
  public:
    ELM(byte RX, byte TX);
    void begin(int UARTBAUD);
    boolean available_pids[256];
    
    // reset
    boolean reset();
    // available_pids
    String get_available_pids(); /* gibt die verfügbaren PIDs als kommagetrennte Liste zurück,
                                    schreibt supported_pids */ 
    // read_all
    // gibt's in der Library nicht, wird im Arduino-Sketch umgesetzt
    // read_pid
    String get_pid_rawdata(byte pid);
    String get_pid_data(byte pid);
    String get_pid_unit(byte pid);
    String get_pid_description(byte pid);
    // read_car
    String get_vin();
    String get_ecu();
    String get_voltage();
    // read_dct
    String get_dtc();
    // delete_dct
    boolean clear_dtc();


    // old
    //void get_available_pids();
    //String get_pid(byte id);
    //void clearDTC();
    //void readDTC();
};

#endif