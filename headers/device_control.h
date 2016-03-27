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


#ifndef __DEV_CTRL_
#define __DEV_CTRL_

#include "application.h"
#include "auto_off_on.h"

#define     SMART_PLUG_PIN D7

// Timer used is uint16_t, so a max of 65535 is possible, for safety restricted
//    to 15 Hours
#define     MAX_ONE_TIME_ON_OFF_TIMER 54000

extern      Timer t_switch_on;
extern      Timer t_switch_off;
extern      int led2;

// extern unsigned long last_on_time;
// extern unsigned long last_off_time;

void        init_device_control();
bool        is_switch_on();
void        register_dev_cntrl_cloud_functions();
void        switch_on();
void        switch_off();

int         f_switch_on(String t_str);
int         f_switch_off(String t_str);

#endif
