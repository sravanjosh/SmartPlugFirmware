/**
  ******************************************************************************
  * @file    voodoospark.cpp
  * @author  Chris Williams
  * @version V4.0.0
  * @date    08-March-2016
  * @brief   Exposes the firmware level API through a TCP Connection initiated
  *          to the Particle devices (Core and Photon)
  ******************************************************************************
  Copyright (c) 2016 Chris Williams (voodootikigod)  All rights reserved.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
  ******************************************************************************
  */

#include "voodoospark.h"
#include "device_control.h"
#include "auto_off_on.h"

uint8_t bytesToExpectByAction[] = {
  0, //keepalive (0x00)
  /* Device Control */
  2, //f_switch_on (0x01)
  2, //f_switch_off (0x02)
  0, //Reserved
  0, //Reserved
  0, //Reserved

  /* auto_on_off */
  1, //f_auto_on
  1, //f_auto_off
  0, //Reserved
  0, //Reserved
  0, //Reserved

};

TCPServer server = TCPServer(PORT);
TCPClient client;

volatile bool hasAction   = false;
volatile bool isConnected = false;

byte analogReporting[20];
byte buffer[MAX_DATA_BYTES];
byte cached[64];
byte pinModeFor[20];
byte portValues[2];
byte reporting[20];

int action, available;
int bytesRead = 0;
int bytesExpecting = 0;
int reporters = 0;

int lastKeepaliveTime          = 0;
Thread* monitorClientThread;


void cacheBuffer(int byteCount, int cacheLength) {
  // Copy the expected bytes into the cache and shift
  // the unused bytes to the beginning of the buffer
  #if _DEBUG
  Serial.print("Cached: ");
  #endif

  for (int k = 0; k < byteCount; k++) {
    // Cache the bytes that we're expecting for
    // this action.
    if (k < cacheLength) {
      cached[k] = buffer[k];

      #if _DEBUG
      Serial.print("0x");
      Serial.print(cached[k], HEX);
      Serial.print(", ");
      #endif
    }

    // Shift the unused buffer to the front
    buffer[k] = buffer[k + cacheLength];
  }

  #if _DEBUG
  Serial.println("");
  #endif
}

void restore() {
  #if _DEBUG
  Serial.println("--------------RESTORING");
  #endif

  hasAction = false;
  isConnected = false;

  reporters = 0;
  bytesRead = 0;
  bytesExpecting = 0;

  memset(&analogReporting[0], 0, 20);
  memset(&buffer[0], 0, MAX_DATA_BYTES);
  memset(&cached[0], 0, 64);
  memset(&pinModeFor[0], 0, 20);
  memset(&portValues[0], 0, 2);
  memset(&reporting[0], 0, 20);

  // Restore defaults.
  for (int i = 0; i < 8; i++) {
    pinMode(i, OUTPUT);
    pinMode(i + 10, INPUT);

    pinModeFor[i] = 1;
    pinModeFor[i + 10] = 0;
  }
}

void processInput() {
  int pin, mode, val, address, reg, delayTime, dataLength;
  int byteCount = bytesRead;

  unsigned long us;

  #if _DEBUG
  Serial.println("--------------PROCESSING");
  #endif

  // Only check if buffer[0] is possibly an action
  // when there is no action in progress.
  if (hasAction == false) {
    if (buffer[0] < ACTION_RANGE) {
      action = buffer[0];
      bytesExpecting = bytesToExpectByAction[action] + 1;
      hasAction = true;

      #if _DEBUG
      Serial.print("Bytes Read: ");
      Serial.println(bytesRead, DEC);
      Serial.print("Bytes Required: ");
      Serial.println(bytesExpecting, DEC);
      Serial.print("Bytes Remaining: ");
      Serial.println(bytesRead - bytesExpecting, DEC);
      #endif
    }
  }

  if ((bytesRead - bytesExpecting) < 0) {
    hasAction = false;
    bytesExpecting = 0;

    #if _DEBUG
    Serial.println("Not Enough Bytes.");
    #endif
    return;
  }

  // When the first byte of buffer is an action and
  // enough bytes are read, begin processing the action.
  if (hasAction && bytesRead >= bytesExpecting) {

    cacheBuffer(byteCount, bytesExpecting);
    byteCount -= bytesExpecting;

    #if _DEBUG
    Serial.print("ACTION: 0x");
    Serial.println(action, HEX);
    #endif

    // Proceed with action processing
    switch (action) {

      case KEEPALIVE:
          lastKeepaliveTime = Time.local();

          // Acknowledge the KEEPALIVE
          server.write((byte)0x00);

          break;
      case SWITCH_ON:
          {
            uint16_t time = (uint16_t)(cached[1] | (cached[2] << 8));

            #if _DEBUG
            Serial.printf("\nSwitch On: Time: %d", time);
            #endif

            if (f_switch_on(String(time, DEC)) == 0) {
              server.write((byte)0x00);
            }
          }

        break;
      case SWITCH_OFF:
          {
            uint16_t time = (uint16_t)(cached[1] | (cached[2] << 8));

            #if _DEBUG
            Serial.printf("\nSwitch Off: Time: %d", time);
            #endif

            if(f_switch_off(String(time, DEC)) == 0) {
              server.write((byte)0x00);
            }
          }

        break;
      case AUTO_ON_TIME:
          {
            #if _DEBUG
            Serial.printf("\nAuto On: Time: %d", cached[1]);
            #endif

            if(f_auto_on(String(cached[1], DEC)) == 0) {
              server.write((byte)0x00);
            }
          }

        break;
      case AUTO_OFF_TIME:
          {
            #if _DEBUG
            Serial.printf("\nAuto Off: Time: %d", cached[1]);
            #endif

            if(f_auto_off(String(cached[1], DEC)) == 0) {
              server.write((byte)0x00);
            }
          }

        break;
      default: // noop
        break;
    } // <-- This is the end of the switch

    memset(&cached[0], 0, 64);

    #if _DEBUG
    Serial.print("Unprocessed Bytes: ");
    Serial.println(byteCount, DEC);
    #endif


    // Reset hasAction flag (no longer needed for this opertion)
    // action and byte read expectation flags
    hasAction = false;
    bytesExpecting = 0;

    // If there were leftover bytes available,
    // call processInput. This mechanism will
    // continue until the buffer is exhausted.

    bytesRead = byteCount;

    if (byteCount > 2) {
      #if _DEBUG
      Serial.println("Calling processInput ");
      #endif

      processInput();
    } else {
      #if _DEBUG
      Serial.println("RETURN TO LOOP!");
      #endif
    }
  }
}

void _setup() {

  server.begin();

  #if _DEBUG
  Serial.begin(115200);
  #endif

  IPAddress ip = WiFi.localIP();
  static char ipAddress[24] = "";

  // https://community.particle.io/t/network-localip-to-string-to-get-it-via-spark-variable/2581/5
  sprintf(ipAddress, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], PORT);

  Particle.variable("endpoint", ipAddress, STRING);

  t_voodootimer.start();
  // monitorClientThread = new Thread("monitorClientThread", monitorClients, NULL);
}

void _loop() {
  if (client.connected()) {

    if (!isConnected) {
      restore();
      lastKeepaliveTime = Time.local();
      #if _DEBUG
      Serial.println("--------------CONNECTED");
      #endif
    } else {
      if((Time.local() - lastKeepaliveTime) >= KEEPALIVE_TIMEOUT) {
        #if _DEBUG
        Serial.println("--------------KEEPALIVE EXPIRED, CLOSED");
        #endif

        client.flush();
        client.stop();
        restore();
        return;
      }
    }

    isConnected = true;

    // Process incoming bytes first
    available = client.available();

    if (available > 0) {

      int received = 0;

      #if _DEBUG
      Serial.println("--------------BUFFERING AVAILABLE BYTES");
      Serial.print("Byte Offset: ");
      Serial.println(bytesRead, DEC);
      #endif

      // Move all available bytes into the buffer,
      // this avoids building up back pressure in
      // the client byte stream.
      for (int i = 0; i < available && i < MAX_DATA_BYTES - bytesRead; i++) {
        buffer[bytesRead++] = client.read();
        received++;
      }

      #if _DEBUG
      Serial.print("Bytes Received: ");
      Serial.println(received, DEC);

      Serial.print("Bytes In Buffer: ");
      Serial.println(bytesRead, DEC);

      for (int i = 0; i < bytesRead; i++) {
        Serial.print(i, DEC);
        Serial.print(":0x");
        Serial.print(buffer[i], HEX);
        Serial.print(", ");
      }
      Serial.println("");
      #endif

      processInput();
    }
  } else {
    // Upon disconnection, restore init state.
    if (isConnected) {
      restore();
    }

    // If no client is yet connected, check for a new connection
    client = server.available();
  }
}

Timer t_voodootimer(1000, _loop);
