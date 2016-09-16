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

#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "globals.h"
#include <vector>

#define EVERY -1
using namespace std;

class Schedule {
public:
  /* Start Date */

  int s_day = EVERY, s_hour = EVERY, s_min = EVERY, s_sec = EVERY;
  // bool s_day_of_week[7] = {0};
  // Left most bit of s_day_of_week is unused
  uint8_t s_day_of_week;

  /* End date - Optional and action will be negated at this time */
  int e_day = EVERY, e_hour = EVERY, e_min = EVERY, e_sec = EVERY;
  // bool e_day_of_week[7] = {0};

  bool recurring = false;
  DeviceActionEnum action;
  OnTick_t switchOnFunction, switchOffFunction;

  Schedule();

  void set_on_function(OnTick_t onTickHandler);
  void set_off_function(OnTick_t onTickHandler);
  void set_action(DeviceActionEnum act) { action = act; }

  vector<AlarmID_t> start();
};

#endif
