/*************************************************************************
 *
 * LINKCONNETWORKS CONFIDENTIAL
 * __________________
 *
 *  2016 - LinkConNetworks Inc.
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of LinkCon Networks Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to LinkCon Networks Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from LinkCon Networks Incorporated.
 *
 * Author: Joshi Sravan Kumar K
 * LinkCon SmartPlug
 *
 * Controlling the device's input/output pins to switch_on and switch_off the
 * smart plug.
 */
#ifndef _GLOBALS__H
#define _GLOBALS__H

#include "TimeAlarms.h"
#include "application.h"

#define _DEBUG 1

#ifdef _DEBUG
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINTF(...)                                                      \
  {}
#define DEBUG_PRINT(x)                                                         \
  {}
#define DEBUG_PRINTLN(x)                                                       \
  {}
#endif

// In Hertz. TODO:Ideally we should be detecting this
#define AC_LINE_FREQUENCY 60
#define DIMMER_LEVELS 128 // 0 - 128 Levels of Dimming.
#define TRIAC_PROP_DELAY                                                       \
  (1000 / (2 * AC_LINE_FREQUENCY)) // Triac propagation delay. Micro seconds
#define DIMMER_STEP_TIME (TRIAC_PROP_DELAY / DIMMER_LEVELS)

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
const int ZCD_PINS[MAX_SUPPORTED_PINS] = {D7, A1, A2, A3, A0, A1, A2, A3};
const bool DIMMER_SUPPORTED[MAX_SUPPORTED_PINS] = {false, true, true, true,
                                                   true,  true, true, true};

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
