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
#include "TimeAlarms.h"
#include "application.h"
#include "device_control.h"
#include "soft_ap_button.h"
#include "auto_off_on.h"
#include "alarm_rules.h"
#include "voodoospark.h"
#include "MDNS.h"
#include <vector>

SYSTEM_THREAD(ENABLED);

#define HTTP_PORT 80
#define ALT_HTTP_PORT 8080

MDNS mdns;

void _mdns_queries() {
  mdns.processQueries();
}

Timer t_mdns_timer(100, _mdns_queries);

void setup()
{
    Serial.begin(9600);
    Time.zone(+5.5);
    /*Particle.syncTime();*/
    /* 1. Device Control */
    init_device_control();
    register_dev_cntrl_cloud_functions();

    /* 2. Soft AP Button Init
    init_soft_ap_button();*/

    /* 3. Alarms */
    //load_alarms();
    /*t_monitor_alarms.start();*/
    Alarm.timerRepeat(2, monitor_alarms);
    register_alarm_cloud_functions();

    /* 4. Auto On Off */
    load_auto_on_time();
    load_auto_off_time();
    register_auto_off_on_cloud_functions();

    /* 5. voodoospark */
    _setup();
    Serial.printf("\nSetup Done");

    Serial.println(WiFi.localIP());

    bool success = mdns.setHostname("core-1");

    std::vector<String> subServices;
    subServices.push_back("smartplug");
    if (success) {
      success = mdns.addService("tcp", "http", 80, "core-1", subServices);
    }

    mdns.addTXTEntry("normal");

    if (success) {
      success = mdns.begin();
      t_mdns_timer.start();
    }
}

void loop() {
  Alarm.delay(1000);
}
