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

#ifndef __AUTO_OFF_ON_H_
#define __AUTO_OFF_ON_H_

#include "device_control.h"

#define AUTO_OFF_MEM_LOCATION 180
#define AUTO_ON_MEM_LOCATION 181

// auto_on_off_time is uint8_t, so a maximum of 255 can be present. To be safe kept 200
#define MAX_AUTO_OFF_ON_TIME 200
#define MIN_AUTO_OFF_ON_TIME 1
#define AUTO_OFF_ON_NEVER 0

// 0 Means never and unit is minutes and a maximum of 200 mins
extern uint8_t auto_off_time;
extern uint8_t auto_on_time;

void persist_auto_off_time(uint8_t time);
void persist_auto_on_time(uint8_t time);
void load_auto_off_time();
void load_auto_on_time();

void start_auto_off();
void stop_auto_off();
void start_auto_on();
void stop_auto_on();

extern Timer t_auto_off;
extern Timer t_auto_on;

int f_auto_off(String t_str);
int f_auto_on(String t_str);
void register_auto_off_on_cloud_functions();

#endif
