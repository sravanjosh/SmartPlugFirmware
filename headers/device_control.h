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

#ifndef __DEV_CTRL_H
#define __DEV_CTRL_H

#include "globals.h"
#include "schedules.h"
#include <map>
#include <mutex>
#include <vector>

using namespace std;

#define DEVICE_INFO_MEM_LOCATION 100

class SmartLoad {

public:
  int pin;
  bool status = false;
  int auto_on_time = 0, auto_off_time = 0;

  static std::map<AlarmID_t, struct ScheduleSmartLoad *> alarmIdToScheduleMap;

  mutex on_off_mutex;
  Timer *delayedOnTimer, *delayedOffTimer, *autoOnTimer, *autoOffTimer;

  // This magic number helps us to figure out whether
  //   after loading the SmartLoad obj from EEPROM, it is actually loaded
  //   or we got some junk data
  volatile uint16_t magic_number = SMART_LOAD_MAGIC_NUMBER;

  vector<Schedule *> schedulesVector;
  SmartLoad() {
    delayedOnTimer = new Timer(1000, [&]() { return switch_on(); }, true);
    delayedOffTimer = new Timer(1000, [&]() { return switch_off(); }, true);

    autoOnTimer = new Timer(1000, [&]() { return switch_on(); }, true);
    autoOffTimer = new Timer(1000, [&]() { return switch_off(); }, true);
  }

  void set_pin(int pin);

  void set_auto_on_time(int minutes) {
    auto_on_time = minutes;
    if (auto_on_time <= 0) {
      stop_auto_on();
    }
  }

  void set_auto_off_time(int minutes) {
    auto_off_time = minutes;
    if (auto_off_time <= 0) {
      stop_auto_off();
    }
  }

  static void manage_alarms();
  static void manage_alarms_negate();

  bool createSchedule(Schedule *schedule);

  bool delete_schedule(int index);

  void switch_on(int afterSeconds);
  void switch_off(int afterSeconds);

  void switch_on();
  void switch_off();
  bool is_switch_on();

  void start_auto_on();
  void start_auto_off();
  void stop_auto_on();
  void stop_auto_off();
};

struct ScheduleSmartLoad {
  Schedule *schedule;
  SmartLoad *smartLoad;
};

#endif
