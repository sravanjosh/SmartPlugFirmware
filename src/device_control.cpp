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

void SmartLoad::set_zcd_pin(int pin) {
  if (!this->is_dimmable) {
    DEBUG_PRINTF("set_zcd_pin:This device doesn't support dimming: %d, "
                 "ignoring this input",
                 this->pin);
    return;
  }
  this->pin = pin;
  pinMode(pin, INPUT);
  attachInterrupt(this->zcd_pin, &SmartLoad::zero_cross_interrupt, this,
                  RISING);
}

/*
int dimtime = (75*dimming);    // For 60Hz =>65
  delayMicroseconds(dimtime);    // Off cycle
  digitalWrite(AC_LOAD, HIGH);   // triac firing
  delayMicroseconds(10);         // triac On propagation delay (for 60Hz use
8.33)
  digitalWrite(AC_LOAD, LOW);    // triac Off
*/
void SmartLoad::zero_cross_interrupt() {
  if (!this->is_dimmable) {
    DEBUG_PRINTF("zero_cross_interrupt:This device doesn't support dimming: "
                 "%d, ignoring this interrupt",
                 this->pin);
    return;
  }
  triac_down_timer->changePeriodFromISR((DIMMER_STEP_TIME * this->dim_value) /
                                        1000);
  triac_down_timer->startFromISR();
}

void SmartLoad::dim_to(int dim_value) { this->dim_value = dim_value; }

void SmartLoad::switch_on(int afterSeconds) {
  if (afterSeconds < 0) {
    DEBUG_PRINTF("\nswitch_on(seconds): Seconds argument shouldn't be "
                 "negative. Ignoring this: %d",
                 afterSeconds);
    afterSeconds = 0;
  }

  DEBUG_PRINTF("\nswitch_on(seconds): Switching On in %d seconds",
               afterSeconds);

  delayedOffTimer->stop();
  delayedOnTimer->changePeriod(afterSeconds * 1000);
  delayedOnTimer->reset();
}

void SmartLoad::switch_off(int afterSeconds) {
  if (afterSeconds < 0) {
    DEBUG_PRINTF("\nswitch_off(seconds): Seconds argument shouldn't be "
                 "negative. Ignoring this: %d",
                 afterSeconds);
    afterSeconds = 0;
  }
  DEBUG_PRINTF("\nswitch_off(seconds): Switching Off in %d seconds",
               afterSeconds);

  delayedOnTimer->stop();
  delayedOffTimer->changePeriod(afterSeconds * 1000);
  delayedOffTimer->reset();
}

void SmartLoad::switch_on() {
  on_off_mutex.lock();
  DEBUG_PRINTF("\nswitch_on: Switching On, PIN: %d", pin);

  if (this->is_dimmable) {
    this->dim_to(0);
  } else {
    digitalWrite(pin, HIGH);
  }

  stop_auto_on();
  start_auto_off();

  on_off_mutex.unlock();
}

void SmartLoad::switch_off() {
  on_off_mutex.lock();
  DEBUG_PRINTF("\nswitch_off: Switching Off, PIN: %d", pin);

  if (this->is_dimmable) {
    this->dim_to(DIMMER_LEVELS);
  } else {
    digitalWrite(pin, LOW);
  }

  stop_auto_off();
  start_auto_on();

  on_off_mutex.unlock();
}

bool SmartLoad::is_switch_on() {
  if (this->is_dimmable) {
    return (this->dim_value < DIMMER_LEVELS);
  } else {
    return (digitalRead(pin) == HIGH) ? true : false;
  }
}

void SmartLoad::start_auto_on() {
  if (auto_on_time > 0 && !is_switch_on()) {
    DEBUG_PRINTF("\nstart_auto_on: Auto on Started %d",
                 (auto_on_time * 60 * 1000));
    autoOnTimer->changePeriod(auto_on_time * 60 * 1000);
    autoOnTimer->reset();
  } else {
    Serial.printf("\nstart_auto_on: Auto on not Started");
  }
}
void SmartLoad::start_auto_off() {
  if (auto_off_time > 0 && is_switch_on()) {
    DEBUG_PRINTF("\nstart_auto_off: Auto off Started %d",
                 (auto_off_time * 60 * 1000));
    autoOffTimer->changePeriod(auto_off_time * 60 * 1000);
    autoOffTimer->reset();
  } else {
    Serial.printf("\nstart_auto_off: Auto off not Started");
  }
}
void SmartLoad::stop_auto_on() { autoOnTimer->stop(); }
void SmartLoad::stop_auto_off() { autoOffTimer->stop(); }

void SmartLoad::manage_alarms() {
  DEBUG_PRINTF("\nmanage_alarms: Entered Manage Alarms");

  AlarmID_t alarm_id = Alarm.getTriggeredAlarmId();
  if (alarmIdToScheduleMap.find(alarm_id) != alarmIdToScheduleMap.end()) {
    struct ScheduleSmartLoad *scheduleSmartLoad =
        SmartLoad::alarmIdToScheduleMap[alarm_id];
    DEBUG_PRINTF("\nmanage_alarms: Schedule H:%d, M:%d, ",
                 scheduleSmartLoad->schedule->s_hour,
                 scheduleSmartLoad->schedule->s_min);

    if (scheduleSmartLoad->schedule->action == ACTION_ON) {
      DEBUG_PRINTF("\nmanage_alarms: Switching ON");

      scheduleSmartLoad->smartLoad->switch_on();
    } else if (scheduleSmartLoad->schedule->action == ACTION_OFF) {
      DEBUG_PRINTF("\nmanage_alarms: Switching OFF");

      scheduleSmartLoad->smartLoad->switch_off();
    }
  }
}

void SmartLoad::manage_alarms_negate() {
  DEBUG_PRINTF("\nmanage_alarms: Entered Manage Alarms");

  AlarmID_t alarm_id = Alarm.getTriggeredAlarmId();
  if (alarmIdToScheduleMap.find(alarm_id) != alarmIdToScheduleMap.end()) {
    struct ScheduleSmartLoad *scheduleSmartLoad =
        SmartLoad::alarmIdToScheduleMap[alarm_id];
    DEBUG_PRINTF("\nmanage_alarms: Schedule H:%d, M:%d, ",
                 scheduleSmartLoad->schedule->s_hour,
                 scheduleSmartLoad->schedule->s_min);

    if (scheduleSmartLoad->schedule->action == ACTION_ON) {
      DEBUG_PRINTF("\nmanage_alarms: Switching ON");

      scheduleSmartLoad->smartLoad->switch_off();
    } else if (scheduleSmartLoad->schedule->action == ACTION_OFF) {
      DEBUG_PRINTF("\nmanage_alarms: Switching OFF");

      scheduleSmartLoad->smartLoad->switch_on();
    }
  }
}

bool SmartLoad::createSchedule(Schedule *schedule) {
  if (schedulesVector.size() < MAX_SCHEDULES_PER_DEVICE) {
    DEBUG_PRINTF("\ncreateSchedule: Creating Schedule");

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
  DEBUG_PRINTF("\ncreateSchedule: Not Creating Schedule, already %d",
               schedulesVector.size());

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
