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
#include "globals.h"
#include "sp_manager.h"

SYSTEM_THREAD(ENABLED);

void show_schedules() {
  Serial.printf("\nSchedules: %d",
                SpManager::instance()->smartLoads[0]->schedulesVector.size());
#if _DEBUG == 1
  Serial.printf("\nCur Time: %s", Time.timeStr().c_str());
/*Serial.printf("\nSize Of SmartLoad: %d", sizeof(SmartLoad));*/
#endif
}

void setup() {
  Serial.begin(9600);
  Time.zone(+5.5);

  Particle.syncTime();

  System.set(SYSTEM_CONFIG_SOFTAP_PREFIX, "LinkPlug");

#if _DEBUG == 1
  delay(5000);
#endif
  SpManager::instance()->init();

  Alarm.timerRepeat(3, show_schedules);
}

int now = Time.now();
void loop() {
  if ((Time.now() - now) > 3600) {
    /*More Than An Hour Syncing Time, Let's do that*/
    Particle.syncTime();
    now = Time.now();
  }

  /**/
  Alarm.delay(1000);
}
