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
 * herein are proprietary to LinkConNetworks Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from LinkConNetworks Incorporated.
 *
 * Author: Joshi Sravan Kumar K
 * LinkCon SmartPlug
 *
 * Controlling the device's input/output pins to switch_on and switch_off the
 * smart plug.
 */

#include <vector>
#include <mutex>
#include <math.h>

#include "device_control.h"
#include "alarm_rules.h"

#define GET_ALARM_ADDRESS(INDEX) (ALARMS_START_ADDRESS + INDEX * (sizeof(struct alarm_time)))

/*
  Mask that says which alarm_address has valid alarm.
  Starting from left(MSb) of the byte 1-occupied, 0-unoccupied
*/
volatile uint8_t alarms_addr_mask = 0x00;
struct alarm_time *alarms[NO_ALARMS_SUPPORTED];
std::mutex alarm_eeprom_mutex;

int8_t get_free_alarm_index() {

  alarm_eeprom_mutex.lock();

  uint8_t mask = 0x80;
  int8_t counter = 0;

  Serial.printf("\nget_free_alarm_index: Outside: %x, %x, %x", mask,\
                   alarms_addr_mask, alarms_addr_mask & mask);
  while((alarms_addr_mask & mask) != 0x0) {
    Serial.printf("\nget_free_alarm_index: Inside: %x, %x",\
                     mask, alarms_addr_mask & mask);
    mask >>= 1;
    ++counter;

    if (mask == 0x0 || counter >= NO_ALARMS_SUPPORTED) {
      counter = -1;
      break;
    }
  }
  alarm_eeprom_mutex.unlock();

  Serial.printf("\nget_free_alarm_index: Free alarm index: %d", counter);
  return counter;
}

void load_alarms() {
  for (int i=0; i<NO_ALARMS_SUPPORTED;i++) {

    alarms[i] = (struct alarm_time*)malloc(sizeof(struct alarm_time));
    if (alarms[i] == NULL) {
      Serial.printf("\nOut of Memory, couldn't load alarms");
      return;
    }
    if(!get_alarm(i, alarms[i])) {
      free(alarms[i]);
      alarms[i] = NULL;
    }
  }
}

void print_alarm(int8_t index) {
  if (index < 0 || index >= NO_ALARMS_SUPPORTED)
      return;

  struct alarm_time *alarm = (struct alarm_time*)malloc(sizeof(struct alarm_time));
  if(alarm == NULL) {
    Serial.printf("\nOut of Memory, couldn't fetch alarm for print");
    return;
  }
  if (!get_alarm(index, alarm)) {
    Serial.printf("\nprint_alarm: No Alarm found");
    free(alarm);
    return;
  }

  Serial.printf("\nIndex: %d, Year: %d, Mon: %d, Day: %d,\
                   Hour: %d, Min: %d, Sec: %d,\
                   DOW: %x, Action: %d", index, alarm->year, alarm->month,
                   alarm->day, alarm->hour, alarm->min, alarm->sec,
                   alarm->dow_mask,
                   (int)alarm->action);
}

void print_alarm(struct alarm_time *alarm) {
  if (alarm == NULL) {
    Serial.println("\nprint_alarm: NULL");
    return;
  }
  Serial.printf("\nYear: %d, Mon: %d, Day: %d, Hour: %d, Min: %d, Sec: %d, DOW: %x, Action: %d",
                   alarm->year, alarm->month,
                   alarm->day, alarm->hour, alarm->min, alarm->sec,
                   alarm->dow_mask,
                   (int)alarm->action);
}

bool get_alarm(int8_t index, struct alarm_time *alarm) {
  if (index < 0 || index >= NO_ALARMS_SUPPORTED)
      return FALSE;

  alarm_eeprom_mutex.lock();

  if ((alarms_addr_mask & (0x80>>index)) == 0x00 ||
      index >= NO_ALARMS_SUPPORTED) {
    return FALSE;
  }

  EEPROM.get(GET_ALARM_ADDRESS(index), *alarm);

  alarm_eeprom_mutex.unlock();

  return TRUE;
}

int8_t persist_alarm(struct alarm_time *alarm) {
    int8_t index = get_free_alarm_index();
    if(index == -1) {
        return -1;
    }

    alarm_eeprom_mutex.lock();

    EEPROM.put(GET_ALARM_ADDRESS(index), *alarm);
    alarms_addr_mask |= 0x80>>index;
    if (alarms[index] != NULL) {
      free(alarms[index]);
    }
    alarms[index] = alarm;
    alarm_eeprom_mutex.unlock();

    return index;
    /*_meta_data.no_alarms_added++;
    persist_metadata();*/
}

int8_t delete_alarm(int8_t index) {
  if (index < 0 || index >= NO_ALARMS_SUPPORTED)
      return -1;

  alarm_eeprom_mutex.lock();

  alarms_addr_mask &= ~(0x80>>index);
  if(alarms[index] != NULL){
    free(alarms[index]);
  }
  alarms[index] = NULL;

  alarm_eeprom_mutex.unlock();

  return 0;
}

void delete_all_alarms() {
  alarm_eeprom_mutex.lock();

  alarms_addr_mask = 0x0;
  for(int i = 0; i < NO_ALARMS_SUPPORTED; i++){
    if (alarms[i] != NULL)
      free(alarms[i]);
    alarms[i] = NULL;
  }

  alarm_eeprom_mutex.unlock();
}

// CLOUD FUNCTIONS

/*
    '*' - Delete all alarms
    "index" delete alarm at index. Should be in range of 0-NO_ALARMS_SUPPORTED
*/
int f_delete_alarm(String t_str) {
  if (t_str == "*") {
    delete_all_alarms();
  } else {

    char *endptr;

    int8_t index = (int8_t)strtol(t_str.c_str(), &endptr, 10);
    if ((errno != 0 && index == 0) || (endptr == t_str.c_str())) {
      return -1;
    }

    return delete_alarm(index);
  }

  return 0;
}

/* Clound function takes a max of 63 characters, need to be careful

    yyyy:m:day:hh:min:sec:<dow>:ON|OFF:

    Each of them can be '*'
    <dow> - one or more days-of-week (0 - SUNDAY, 6 - SATURDAY)

    Should end with ':'

    e.g., 2016:9:10:11:20:*:01:ON:
*/
int f_create_alarm(String t_str){
    Serial.printf("\nIn f_create_alarm: %s", t_str.c_str());
    if(get_free_alarm_index() == -1) {
      return -1;
    }

    //TODO: Validation of the string input

    std::string s = t_str.c_str();
    size_t pos = 0;
    std::string token;

    struct alarm_time *alarm = (struct alarm_time*)malloc(sizeof(struct alarm_time));
    if(alarm == NULL) {
      Serial.printf("\nOut of Memory, couldn't create alarm");
      return -1;
    }

    char *endptr;

    int count = 0;
    while ((pos = s.find(CLOUD_TIME_STAMP_DELIM)) != std::string::npos) {
        token = s.substr(0, pos);

        switch(count++){
            case 0:
                    alarm->year = (token == "*")?-1:(int16_t)strtol(token.c_str(), &endptr, 10);
                    if ((errno != 0 && alarm->year == 0) || (endptr == token.c_str())){

                      Serial.printf("\nIn f_create_alarm:Bad year: %d",
                            alarm->year);
                      free(alarm);
                      return -1;
                    }

                    if(alarm->year < Time.year(Time.local())) {
                      Serial.printf("\nIn f_create_alarm:Year is in past, year: %d",
                            alarm->year);
                      free(alarm);
                      return -1;
                    }

                    Serial.printf("\nIn f_create_alarm:token %s, year: %d",
                          token.c_str(), alarm->year);
                    break;
            case 1:
                    Serial.printf("\nToken Month: %s", token.c_str());
                    alarm->month = (token == "*")?-1:(int8_t)strtol(token.c_str(), &endptr, 10);
                    if ((errno != 0 && alarm->month == 0) ||
                        (endptr == token.c_str()) ||
                        alarm->month < 0){

                      Serial.printf("\nIn f_create_alarm:Bad month: %d",
                            alarm->month);
                      free(alarm);
                      return -1;
                    }

                    Serial.printf("\nIn f_create_alarm:token %s, month: %d",
                          token.c_str(), alarm->month);
                    break;
            case 2:
                    Serial.printf("\nToken Day: %s", token.c_str());
                    alarm->day = (token == "*")?-1:(int8_t)strtol(token.c_str(), &endptr, 10);
                    if ((errno != 0 && alarm->day == 0) || (endptr == token.c_str())){

                      Serial.printf("\nIn f_create_alarm:Bad day: %d",
                            alarm->day);
                      free(alarm);
                      return -1;
                    }

                    Serial.printf("\nIn f_create_alarm:token %s, hour: %d",
                          token.c_str(), alarm->day);
                    break;
            case 3:
                    Serial.printf("\nToken Hour: %s", token.c_str());
                    alarm->hour = (token == "*")?-1:(int8_t)strtol(token.c_str(), &endptr, 10);
                    if ((errno != 0 && alarm->hour == 0) || (endptr == token.c_str())){

                      Serial.printf("\nIn f_create_alarm:Bad hour: %d",
                            alarm->hour);
                      free(alarm);
                      return -1;
                    }

                    Serial.printf("\nIn f_create_alarm:token %s, hour: %d",
                          token.c_str(), alarm->hour);
                    break;
            case 4:
                    Serial.printf("\nToken Min: %s", token.c_str());
                    alarm->min = (token == "*")?-1:(int8_t)strtol(token.c_str(), &endptr, 10);
                    if ((errno != 0 && alarm->min == 0) || (endptr == token.c_str())){

                      Serial.printf("\nIn f_create_alarm:Bad min: %d",
                            alarm->min);
                      free(alarm);
                      return -1;
                    }

                    Serial.printf("\nIn f_create_alarm:token %s, min: %d",
                          token.c_str(), alarm->min);
                    break;
            case 5:
                    Serial.printf("\nToken Sec: %s", token.c_str());
                    alarm->sec = (token == "*")?-1:(int8_t)strtol(token.c_str(), &endptr, 10);
                    if ((errno != 0 && alarm->sec == 0) || (endptr == token.c_str())){

                      Serial.printf("\nIn f_create_alarm:Bad sec: %d",
                            alarm->sec);
                      free(alarm);
                      return -1;
                    }

                    Serial.printf("\nIn f_create_alarm:token %s, sec: %d",
                          token.c_str(), alarm->sec);
                    break;
            case 6:

                    if (token == "*") {
                      alarm->dow_mask = 0xFF;
                    } else {
                      /*
                      SUNDAY --> SATURDAY
                       0     --> 6
                      */
                      alarm->dow_mask = 0x0;
                      if (token.find("0", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x40;
                      if (token.find("1", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x20;
                      if (token.find("2", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x10;
                      if (token.find("3", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x08;
                      if (token.find("4", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x04;
                      if (token.find("5", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x02;
                      if (token.find("6", 0, 1) != std::string::npos)
                            alarm->dow_mask |= 0x01;
                    }
                    Serial.printf("\nIn f_create_alarm:token %s, dow_mask: %x",
                          token.c_str(), alarm->dow_mask);

                    break;
            case 7:
                    if (token == "ON") {
                        alarm->action = ON;
                    } else if (token == "OFF") {
                        alarm->action = OFF;
                    } else {
                        return -1;
                    }
                    Serial.printf("\nIn f_create_alarm:token %s, action: %d",
                          token.c_str(), (int)alarm->action);
                    break;
            default:
                    /* Never hit if the time format sent is correct */
                    break;
        }

        s.erase(0, pos + 1); //Increase by length of delimiter
    }

    if (count <= 7) { // We are expecting this many no of tokens be present
      free(alarm);
      return -1;
    }

    Serial.printf("\nAlarm added at: %d", persist_alarm(alarm));
    return 0;
}

void register_alarm_cloud_functions() {
  Particle.function("create_alarm", f_create_alarm);
  Particle.function("delete_alarm", f_delete_alarm);
}

// Timer t_monitor_alarms(1000, monitor_alarms);
// TimeAlarmsClass alarm_monitor_alarms = TimeAlarmsClass() ;

boolean invoke_alarm(uint8_t index) {
  if (alarms[index] != NULL) {
    if (alarms[index]->action == ON) {
      switch_on();
    } else if(alarms[index]->action == OFF) {
      switch_off();
    } else {
      Serial.printf("\nInvalid action");
    }
  }
}

void monitor_alarms() {
  Serial.printf("\n----- MONITOR ALARM START -------");
  for (uint8_t i=0; i < NO_ALARMS_SUPPORTED;i++) {
    Serial.printf("\n%d:", i);
    print_alarm(alarms[i]);
    unsigned long cur_time = Time.now();
    // Serial.printf("\n%d:%d:%d:%d:%d", cur_time, Time.month(cur_time), Time.day(cur_time), Time.hour(cur_time), Time.minute(cur_time));
    // cur_time = Time.local();
    // Serial.printf("\n%d: %d:%d:%d:%d", cur_time, Time.month(cur_time), Time.day(cur_time), Time.hour(cur_time), Time.minute(cur_time));
    if((alarms[i]->month == -1 || Time.month(cur_time) == alarms[i]->month) &&
       (alarms[i]->day == -1 || Time.day(cur_time) == alarms[i]->day) &&
       (alarms[i]->hour == -1 || Time.hour(cur_time) == alarms[i]->hour) &&
       (alarms[i]->min == -1 || Time.minute(cur_time) == alarms[i]->min) &&
       (alarms[i]->dow_mask == 0xff ||
            (( (int)pow(2, 7-Time.weekday(cur_time)) & alarms[i]->dow_mask ) != 0x0))
      ) {
        // The following if condition makes sure, if min is "*", it will not apply
        //    trigger (say ON) again if the user switches off after the alarm.
        //  This condition reduces this chance.
        if(Time.second(cur_time) >= 0 && Time.second(cur_time) <= 3) {
          invoke_alarm(i);
        }
      }
    Serial.printf("\nmonitor_alarms: Alarm address mask: %x", alarms_addr_mask);
  }
  Serial.printf("\n----- MONITOR ALARM END -------");
}
