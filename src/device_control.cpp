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

#include "device_control.h"

// SmartLoad::SmartLoad(int pin) : pin(pin) {}
std::map<AlarmID_t, struct ScheduleSmartLoad *> SmartLoad::alarmIdToScheduleMap;

void SmartLoad::set_pin(int pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void SmartLoad::switch_on(int afterSeconds) {
  if (afterSeconds < 0) {
#if _DEBUG
    Serial.printf(
        "\nswitch_on(seconds): Seconds argument shouldn't be negative: %d",
        afterSeconds);
#endif
  }

#if _DEBUG
  Serial.printf("\nswitch_on(seconds): Switching On in %d seconds",
                afterSeconds);
#endif
  delayedOffTimer->stop();
  delayedOnTimer->changePeriod(afterSeconds * 1000);
  delayedOnTimer->reset();
}

void SmartLoad::switch_off(int afterSeconds) {
  if (afterSeconds < 0) {
#if _DEBUG
    Serial.printf(
        "\nswitch_off(seconds): Seconds argument shouldn't be negative: %d",
        afterSeconds);
#endif
  }
#if _DEBUG
  Serial.printf("\nswitch_off(seconds): Switching Off in %d seconds",
                afterSeconds);
#endif
  delayedOnTimer->stop();
  delayedOffTimer->changePeriod(afterSeconds * 1000);
  delayedOffTimer->reset();
}

void SmartLoad::switch_on() {
  on_off_mutex.lock();
#if _DEBUG
  Serial.printf("\nswitch_on: Switching On, PIN: %d", pin);
#endif

  digitalWrite(pin, HIGH);

  stop_auto_on();
  start_auto_off();

  on_off_mutex.unlock();
}

void SmartLoad::switch_off() {
  on_off_mutex.lock();
#if _DEBUG
  Serial.printf("\nswitch_off: Switching Off, PIN: %d", pin);
#endif
  digitalWrite(pin, LOW);

  stop_auto_off();
  start_auto_on();

  on_off_mutex.unlock();
}
bool SmartLoad::is_switch_on() {
  return (digitalRead(pin) == HIGH) ? true : false;
}

void SmartLoad::start_auto_on() {
  if (auto_on_time > 0 && !is_switch_on()) {
#if _DEBUG
    Serial.printf("\nstart_auto_on: Auto on Started %d",
                  (auto_on_time * 60 * 1000));
#endif
    autoOnTimer->changePeriod(auto_on_time * 60 * 1000);
    autoOnTimer->reset();
  } else {
    Serial.printf("\nstart_auto_on: Auto on not Started");
  }
}
void SmartLoad::start_auto_off() {
  if (auto_off_time > 0 && is_switch_on()) {
#if _DEBUG
    Serial.printf("\nstart_auto_off: Auto off Started %d",
                  (auto_off_time * 60 * 1000));
#endif
    autoOffTimer->changePeriod(auto_off_time * 60 * 1000);
    autoOffTimer->reset();
  } else {
    Serial.printf("\nstart_auto_off: Auto off not Started");
  }
}
void SmartLoad::stop_auto_on() { autoOnTimer->stop(); }
void SmartLoad::stop_auto_off() { autoOffTimer->stop(); }

void SmartLoad::manage_alarms() {
#if _DEBUG == 1
  Serial.printf("\nmanage_alarms: Entered Manage Alarms");
#endif
  AlarmID_t alarm_id = Alarm.getTriggeredAlarmId();
  if (alarmIdToScheduleMap.find(alarm_id) != alarmIdToScheduleMap.end()) {
    struct ScheduleSmartLoad *scheduleSmartLoad =
        SmartLoad::alarmIdToScheduleMap[alarm_id];
#if _DEBUG == 1
    Serial.printf("\nmanage_alarms: Schedule H:%d, M:%d, ",
                  scheduleSmartLoad->schedule->s_hour,
                  scheduleSmartLoad->schedule->s_min);
#endif
    if (scheduleSmartLoad->schedule->action == ACTION_ON) {
#if _DEBUG == 1
      Serial.printf("\nmanage_alarms: Switching ON");
#endif
      scheduleSmartLoad->smartLoad->switch_on();
    } else if (scheduleSmartLoad->schedule->action == ACTION_OFF) {
#if _DEBUG == 1
      Serial.printf("\nmanage_alarms: Switching OFF");
#endif
      scheduleSmartLoad->smartLoad->switch_off();
    }
  }
}

void SmartLoad::manage_alarms_negate() {
#if _DEBUG == 1
  Serial.printf("\nmanage_alarms: Entered Manage Alarms");
#endif
  AlarmID_t alarm_id = Alarm.getTriggeredAlarmId();
  if (alarmIdToScheduleMap.find(alarm_id) != alarmIdToScheduleMap.end()) {
    struct ScheduleSmartLoad *scheduleSmartLoad =
        SmartLoad::alarmIdToScheduleMap[alarm_id];
#if _DEBUG == 1
    Serial.printf("\nmanage_alarms: Schedule H:%d, M:%d, ",
                  scheduleSmartLoad->schedule->s_hour,
                  scheduleSmartLoad->schedule->s_min);
#endif
    if (scheduleSmartLoad->schedule->action == ACTION_ON) {
#if _DEBUG == 1
      Serial.printf("\nmanage_alarms: Switching ON");
#endif
      scheduleSmartLoad->smartLoad->switch_off();
    } else if (scheduleSmartLoad->schedule->action == ACTION_OFF) {
#if _DEBUG == 1
      Serial.printf("\nmanage_alarms: Switching OFF");
#endif
      scheduleSmartLoad->smartLoad->switch_on();
    }
  }
}

bool SmartLoad::createSchedule(Schedule *schedule) {
  if (schedulesVector.size() < MAX_SCHEDULES_PER_DEVICE) {
#if _DEBUG == 1
    Serial.printf("\ncreateSchedule: Creating Schedule");
#endif
    schedule->set_on_function(&SmartLoad::manage_alarms);
    schedule->set_off_function(&SmartLoad::manage_alarms_negate);
    schedulesVector.push_back(schedule);
    vector<AlarmID_t> alarm_ids = schedule->start();

    struct ScheduleSmartLoad *scheduleSmartLoad = new struct ScheduleSmartLoad;
    scheduleSmartLoad->schedule = schedule;
    scheduleSmartLoad->smartLoad = this;

    for (AlarmID_t alarm_id : alarm_ids) {
      alarmIdToScheduleMap[alarm_id] = scheduleSmartLoad;
    }

    return true;
  }
#if _DEBUG == 1
  Serial.printf("\ncreateSchedule: Not Creating Schedule, already %d",
                schedulesVector.size());
#endif
  return false;
}

bool SmartLoad::delete_schedule(int index) {
  if (index < 0 || index >= schedulesVector.size()) {
    return false;
  }
  delete schedulesVector[index];
  schedulesVector.erase(schedulesVector.begin() + index);

  return true;
}
