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

#ifndef __ALRM_H_
#define __ALRM_H_

#include "application.h"

// Should be a max of 8, as alarms_addr_mask is of int8_t
#define NO_ALARMS_SUPPORTED                         2
#define ALARMS_START_ADDRESS                        200
#define ALARM_ADDR_MASK_ADDRESS                     195

#define CLOUD_TIME_STAMP_DELIM                      ':'
#define CLOUD_DOW_DELIM                             ','

enum SocketAction {
  ON,
  OFF
};

struct alarm_time {
  int16_t year;
  int8_t month, day, hour, min, sec;
  /*
    This is useful as byte comparison will be faster than
    string comparison.
    +-------------------------------+
    | R | R | Y | M | D | H | M | S |
    +-------------------------------+
    R - Reserved
    0 - Not wild card
    1 - Wildcard
  */
  byte time_stamp_wild_card_mask = 0x0;

  /*
    +-------------------------------+
    | R | S | M | T | W | T | F | S |
    +-------------------------------+
    R - Reserved
    0 - Not wild card
    1 - Wildcard
  */
  byte dow_mask = 0x0;
  SocketAction action = ON;
};

extern Timer t_monitor_alarms;

int8_t get_free_alarm_index();
void load_alarms();
void monitor_alarms();

void print_alarm(struct alarm_time *alarm);
void print_alarm(int8_t index);

int8_t delete_alarm(int8_t index);
void delete_all_alarms();
int8_t persist_alarm(struct alarm_time *alarm);
bool get_alarm(int8_t index, struct alarm_time *alarm);

// CLOUD FUNCTIONS;
int f_delete_alarm(String t_str);
int f_create_alarm(String t_str);
void register_alarm_cloud_functions();

#endif
