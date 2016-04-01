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
 * Dissemination of this information or reproduction of this  material
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
#include "TimeAlarms.h"
#include <mutex>

/*int led2 = D7;*/
std::mutex on_off_mutex;

/*unsigned long last_on_time = 0;
unsigned long last_off_time = 0;*/

void init_device_control() {
  pinMode(SMART_PLUG_PIN, OUTPUT);
  switch_off();

  byte mac[6];
  WiFi.macAddress(mac);

  static char mac_address[6];
  sprintf(mac_address, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Particle.variable("mac_address", mac_address, STRING);
}

bool is_switch_on() {
  return (digitalRead(SMART_PLUG_PIN) == HIGH)?TRUE:FALSE;
}

void switch_on() {
    Serial.printf("\nSwitching on the device");

    on_off_mutex.lock();

    digitalWrite(SMART_PLUG_PIN, HIGH);

    stop_auto_on();
    start_auto_off();

    on_off_mutex.unlock();
}

void switch_off() {
    Serial.printf("\nSwitching off the device");

    on_off_mutex.lock();

    digitalWrite(SMART_PLUG_PIN, LOW);

    stop_auto_off();
    start_auto_on();

    on_off_mutex.unlock();
}

Timer t_switch_of(1000, switch_off, true);
Timer t_switch_on(1000, switch_on, true);
// TimeAlarmsClass alarm_switch_off = TimeAlarmsClass() ;
// TimeAlarmsClass alarm_switch_on = TimeAlarmsClass() ;
// **************************
// CLOUD FUNCTIONS
// **************************
/*
  Takes seconds as input. Switch_on after t_str number of seconds
*/
int f_switch_on(String t_str){

    /*int time = atoi(t_str);*/
    char *endptr;
    uint16_t _time = (uint16_t)strtol(t_str.c_str(), &endptr, 10);
    if ((errno != 0 && _time == 0) || (endptr == t_str.c_str())){
      _time = 0;
    }

    if (_time > MAX_ONE_TIME_ON_OFF_TIMER) {
      return -2;
    }

    _time *= 1000;

    t_switch_of.stop();

    if (_time == 0) {
      switch_on();
    } else {
      t_switch_on.changePeriod(_time);
      t_switch_on.reset();
      // Alarm.timerOnce(_time, switch_on);
    }

    return 0;
}

/*
  Takes seconds as input. Switch_off after t_str number of seconds
*/
int f_switch_off(String t_str){

    /*int time = atoi(t_str);*/
    char *endptr;
    uint16_t _time = (uint16_t)strtol(t_str.c_str(), &endptr, 10);
    if ((errno != 0 && _time == 0) || (endptr == t_str.c_str())){
      _time = 0;
    }

    if (_time > MAX_ONE_TIME_ON_OFF_TIMER) {
      return -2;
    }

    _time *= 1000;

    t_switch_on.stop();

    if (_time == 0) {
      switch_off();
    } else {
      t_switch_of.changePeriod(_time);
      t_switch_of.start();
      // Alarm.timerOnce(_time, switch_off);
    }

    return 0;
}

int f_is_switch_on(String t_str) {
  return is_switch_on()?1:0;
}

void register_dev_cntrl_cloud_functions() {
  Particle.function("switch_on", f_switch_on);
  Particle.function("switch_off", f_switch_off);
  Particle.function("is_switch_on", f_is_switch_on);
}
