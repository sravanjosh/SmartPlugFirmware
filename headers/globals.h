#ifndef _GLOBALS__H
#define _GLOBALS__H

#include "TimeAlarms.h"
#include "application.h"

#define _DEBUG 1

#define SKU_LINK_PLUG_1 1
#define SKU_LINK_PLUG_2 2
#define SKU_LINK_PLUG_4 3
#define SKU_LINK_PLUG_8 4

#define SKU_ID SKU_LINK_PLUG_4

#define MAX_SUPPORTED_PINS 8

// Seconds
#define MAX_ONE_TIME_ON_OFF_TIMER 30000
// Minutes
#define MAX_AUTO_OFF_ON_TIME 1200
#define MAX_SCHEDULES_PER_DEVICE 5
#define CLOUD_FN_ARG_DELIM ':'

// This magic number helps us to figure out whether
//   after loading the SmartLoad obj from EEPROM, it is actually loaded
//   or we got some junk data
#define SMART_LOAD_MAGIC_NUMBER 0xdead
#define SMART_LOAD_EEPROM_STARTING_LOCATION 100
#define SMART_LOAD_EEPROM_SIZE 200

volatile extern bool is_away;

// TODO: Need to change the first pin to D0, after testing
// const int SUPPORTED_PINS[MAX_SUPPORTED_PINS] = {D7, D1, D2, D3, D4, D5, D6,
// D7};
const int SUPPORTED_PINS[MAX_SUPPORTED_PINS] = {D7, D4, D5, D6, D4, D5, D6, D7};

enum LoadTypeEnum {
  LOAD_UNKNOWN,
  LOAD_LIGHT,
  LOAD_FAN,
  LOAD_AC,
  LOAD_GEYSER,
  LOAD_COFFEE_MAKER,
  LOAD_CURTAINS,
  LOAD_MOTOR
};

enum DeviceActionEnum { ACTION_ON, ACTION_OFF };

#endif
