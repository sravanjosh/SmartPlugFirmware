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

#include "application.h"
#include "device_control.h"
#include "soft_ap_button.h"
#include "auto_off_on.h"
#include "alarm_rules.h"
#include "voodoospark.h"
#include "MDNS.h"
#include <vector>

SYSTEM_THREAD(ENABLED);

/*TCPServer server = TCPServer(23);
TCPClient client;*/

#define HTTP_PORT 80
#define ALT_HTTP_PORT 8080

MDNS mdns;

/*TCPServer httpServer = TCPServer(HTTP_PORT);
TCPServer altServer = TCPServer(ALT_HTTP_PORT);*/
void _mdns_queries() {
  mdns.processQueries();
}

Timer t_mdns_timer(100, _mdns_queries);

void setup()
{
    /*server.begin();*/
    /*httpServer.begin();
    altServer.begin();*/

    Serial.begin(9600);

    /* Device Control */
    init_device_control();
    register_dev_cntrl_cloud_functions();

    /* Soft AP Button Init */
    /*init_soft_ap_button();*/

    /* Alarms */
    /*load_alarms();
    t_monitor_alarms.start();*/
    register_alarm_cloud_functions();

    /* Auto On Off */
    load_auto_on_time();
    load_auto_off_time();
    register_auto_off_on_cloud_functions();

    _setup();
    Serial.printf("\nSetup Done");

    Serial.println(WiFi.localIP());


    /* MDNS Stuff */
    /*std::vector<String> subServices;*/

    /*subServices.push_back("light");*/

    bool success = mdns.setHostname("core-1");

    /*if (success) {
      success = mdns.addService("tcp", "http", HTTP_PORT, "Core 1", subServices);
    }

    mdns.addTXTEntry("normal");

    if (success) {
      success = mdns.addService("tcp", "http", ALT_HTTP_PORT, "Core alt");
    }

    mdns.addTXTEntry("alt");*/

    if (success) {
      success = mdns.begin();
      t_mdns_timer.start();
    }
}

void loop() {
  /*_loop();*/
    /*static bool is_connected = false;
    if (client.connected()) {
      if (!is_connected) {
        Serial.printf("\nClient connected");
      }
      is_connected = true;
      while(client.available()) {
        Serial.printf("%c", client.read());
      }
    } else {
      if (is_connected) {
        Serial.printf("\nClient disconnected");
        client.stop();
        is_connected = false;
      }

      client = server.available();
    }*/

    /*Serial.printf("\nIn Loop");
    delay(500);*/

    /* MDNS Stuff */
    /*mdns.processQueries();*/

    /*TCPClient client = httpServer.available();

    if (client){
      while (client.read() != -1);

      client.write("HTTP/1.1 200 Ok\n\n<html><body><h1>Ok!</h1></body></html>\n\n");
      client.flush();
      delay(5);
      client.stop();
    }

    TCPClient altClient = altServer.available();

    if (altClient){
      while (altClient.read() != -1);

      altClient.write("HTTP/1.1 200 Ok\n\n<html><body><h1>Alternative port ok!</h1></body></html>\n\n");
      altClient.flush();
      delay(5);
      altClient.stop();
    }*/
}
