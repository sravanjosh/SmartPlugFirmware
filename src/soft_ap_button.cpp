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

#include "soft_ap_button.h"

volatile uint32_t msListening;

void init_soft_ap_button() {
  pinMode(SOFT_AP_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(SOFT_AP_BUTTON_PIN, soft_ap_button_ISR, FALLING);
}

void soft_ap_button_ISR() {
 if (millis() - msListening < DEBOUNCE) return;
  msListening = millis();
  // start/stop listening depending on current state

  if (!WiFi.listening())
  {
      Particle.disconnect();
      WiFi.listen();
  }
  else
  {
      WiFi.listen(false);
      Particle.connect();
  }
}
