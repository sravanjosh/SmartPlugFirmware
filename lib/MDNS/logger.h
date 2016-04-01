#ifndef __LOG_H_
#define __LOG_H_

#include "application.h"
#define SERIAL_SPEED 9600
/*
  0 - DEBUG
  1 - INFO
  2 - WARN
  3 - ERROR
  4 - CRITICAL
*/
#define LOG_LEVEL 0

class Logger{
  private:
    bool is_ready = false;

    Logger() {
      get_ready();
    }
    void get_ready() {
      Serial.begin(SERIAL_SPEED);
    }

  public:
    void debug(char* message) {
      #if LOG_LEVEL <= 0
      Serial.printf("\n%s", message);
      #endif
    }

    void info(char* message) {
      #if LOG_LEVEL <= 1
      Serial.printf("\n%s", message);
      #endif
    }

    void warn(char* message) {
      #if LOG_LEVEL <= 2
      Serial.printf("\n%s", message);
      #endif
    }

    void error(char* message) {
      #if LOG_LEVEL <= 1
      Serial.printf("\n%s", message);
      #endif
    }

    void critical(char* message) {
      #if LOG_LEVEL <= 2
      Serial.printf("\n%s", message);
      #endif
    }

};


#endif
