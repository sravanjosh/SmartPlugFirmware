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

#include "schedules.h"

Schedule::Schedule() {}
void Schedule::set_on_function(OnTick_t onTickHandler) {
  switchOnFunction = onTickHandler;
}

void Schedule::set_off_function(OnTick_t onTickHandler) {
  switchOffFunction = onTickHandler;
}

vector<AlarmID_t> Schedule::start() {
#if _DEBUG == 1
  Serial.printf("\nSchedule::start : Alarm monitoring started");
#endif
  vector<AlarmID_t> alarm_ids;
  // if ((s_day_of_week[0] == s_day_of_week[1]) &&
  //     (s_day_of_week[0] == s_day_of_week[2]) &&
  //     (s_day_of_week[0] == s_day_of_week[3]) &&
  //     (s_day_of_week[0] == s_day_of_week[4]) &&
  //     (s_day_of_week[0] == s_day_of_week[5]) &&
  //     (s_day_of_week[0] == s_day_of_week[6])) {

  if (((s_day_of_week & 0xEF) == 0xEF) || ((s_day_of_week | 0x00) == 0x00)) {
    // All false or all true, which means daily
    // Alarm.alarmRepeat(s_hour, s_min, 0, switchOnFunction);
    AlarmID_t id = Alarm.alarmRepeat(s_hour, s_min, 0, switchOnFunction);
    alarm_ids.push_back(id);

    if (e_hour != -1 && e_min != -1) {
      id = Alarm.alarmRepeat(e_hour, e_min, 0, switchOffFunction);
      alarm_ids.push_back(id);
    }
  } else {
    uint8_t mask = 0x80;
    for (int i = 0; i < 7; i++) {
      if (s_day_of_week & (mask >> 1)) {

        AlarmID_t id =
            Alarm.alarmRepeat((i + 1), s_hour, s_min, 0, switchOnFunction);
        alarm_ids.push_back(id);

        if (e_hour != -1 && e_min != -1) {
          if (((e_hour == s_hour) && (e_min < s_min)) || (e_hour < s_hour)) {
            int dow = i + 2;
            if (dow == 8)
              dow = 0;
            id = Alarm.alarmRepeat(dow, e_hour, e_min, 0, switchOffFunction);
          }
        }
      }
    }
  }

  return alarm_ids;
}
