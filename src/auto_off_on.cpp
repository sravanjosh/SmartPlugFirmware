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

#include "auto_off_on.h"
#include "device_control.h"
#include <mutex>

uint8_t auto_off_time = AUTO_OFF_ON_NEVER;
uint8_t auto_on_time = AUTO_OFF_ON_NEVER;
std::mutex auto_off_mutex;
std::mutex auto_on_mutex;

void persist_auto_off_time(uint8_t time) {
  auto_off_mutex.lock();

  EEPROM.put(AUTO_OFF_MEM_LOCATION, time);
  auto_off_time = time;
  /*t_auto_off.changePeriod(auto_off_time * 60000);*/
  Serial.printf("\npersist_auto_off_time: Persisted %d", auto_off_time * 60000);
  auto_off_mutex.unlock();
}

void persist_auto_on_time(uint8_t time) {
  auto_on_mutex.lock();

  EEPROM.put(AUTO_ON_MEM_LOCATION, time);
  auto_on_time = time;

  Serial.printf("\npersist_auto_on_time: Persisted %d", auto_on_time * 60000);
  auto_on_mutex.unlock();
}

void load_auto_off_time() {
  auto_off_mutex.lock();

  EEPROM.get(AUTO_OFF_MEM_LOCATION, auto_off_time);
  Serial.printf("\nload_auto_off_time: auto_off_time %d", auto_off_time);

  auto_off_mutex.unlock();
}

void load_auto_on_time() {
  auto_on_mutex.lock();

  EEPROM.get(AUTO_ON_MEM_LOCATION, auto_on_time);
  Serial.printf("\nload_auto_on_time: auto_on_time %d", auto_on_time);

  auto_on_mutex.unlock();
}

void start_auto_off() {
  auto_off_mutex.lock();

  if (auto_off_time > 0) {
    Serial.printf("\nstart_auto_off: Auto off Started %d", (auto_off_time*60000));
    t_auto_off.changePeriod(auto_off_time * 60000);
    t_auto_off.reset();
  }

  auto_off_mutex.unlock();
}

void stop_auto_off() {
  t_auto_off.stop();
}

void start_auto_on() {
  auto_on_mutex.lock();

  if (auto_on_time > 0) {
    Serial.printf("\nstart_auto_on: Auto on Started %d", (auto_on_time*60000));
    t_auto_on.changePeriod(auto_on_time * 60000);
    t_auto_on.reset();
  }

  auto_on_mutex.unlock();
}

void stop_auto_on() {
  t_auto_off.stop();
}

Timer t_auto_off(1000, switch_off, true);
Timer t_auto_on(1000, switch_on, true);

/* Auto On Time in Minutes */
int f_auto_on(String t_str) {
    char *endptr;

    uint8_t _auto_on_time = (uint8_t)strtol(t_str.c_str(), &endptr, 10);
    if ((errno != 0 && _auto_on_time == 0) || (endptr == t_str.c_str())){
      return -1;
    }

    if (_auto_on_time < 0 || _auto_on_time > MAX_AUTO_OFF_ON_TIME) {
      return -2;
    }

    persist_auto_on_time(_auto_on_time);
    if (!is_switch_on()) {
      start_auto_on();
    }

    return 0;
}

/* Auto Off Time in Minutes */
int f_auto_off(String t_str) {
  char *endptr;

  uint8_t _auto_off_time = (uint8_t)strtol(t_str.c_str(), &endptr, 10);
  if ((errno != 0 && _auto_off_time == 0) || (endptr == t_str.c_str())){
    return -1;
  }

  if (_auto_off_time < 0 || _auto_off_time > MAX_AUTO_OFF_ON_TIME) {
    return -2;
  }

  persist_auto_off_time(_auto_off_time);
  if (is_switch_on()) {
    start_auto_off();
  }

  return 0;
}

void register_auto_off_on_cloud_functions() {
  Particle.function("auto_on", f_auto_on);
  Particle.function("auto_off", f_auto_off);
}
