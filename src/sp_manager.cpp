#include "sp_manager.h"

SpManager *SpManager::spManagerInstance = 0;

void SpManager::switch_on_all() {
  for (SmartLoad *&smart_load : smartLoads) {
    smart_load->switch_on();
  }
}

void SpManager::switch_off_all() {
  for (SmartLoad *&smart_load : smartLoads) {
    smart_load->switch_off();
  }
}

void SpManager::init() {
#if _DEBUG
  Serial.printf("\nSpManager.init: Initialization Started.");
#endif

  for (int i = 0; i < NUMBER_OF_PINS; i++) {
    SmartLoad *smLoad = new SmartLoad;
    EEPROM.get(SMART_LOAD_EEPROM_STARTING_LOCATION +
                   (i * SMART_LOAD_EEPROM_SIZE),
               *smLoad);

    if (smLoad->magic_number != SMART_LOAD_MAGIC_NUMBER) {
#if _DEBUG
      Serial.printf(
          "\nSpManager::init: Magic didn't match, creating new, %d, %u", i,
          smLoad->magic_number);
#endif
      smartLoads[i] = new SmartLoad;
      smartLoads[i]->set_pin(SUPPORTED_PINS[i]);
    } else {
#if _DEBUG
      Serial.printf("\nSpManager::init: Magic matched, not creating new, %d",
                    i);
#endif
      smartLoads[i] = smLoad;
#if _DEBUG
      Serial.printf("\nSpManager::init: Pin %d, AutoOff %d, AutonOn %d ",
                    smartLoads[i]->pin, smartLoads[i]->auto_off_time,
                    smartLoads[i]->auto_on_time);
#endif
      // smartLoads[i] = new SmartLoad;
      // smartLoads[i]->set_pin(SUPPORTED_PINS[i]);
    }
  }

  Particle.function("switch_on", &SpManager::f_switch_on, this);
  Particle.function("switch_off", &SpManager::f_switch_off, this);
  Particle.function("is_switch_on", &SpManager::f_is_switch_on, this);

  Particle.function("auto_on", &SpManager::f_auto_on, this);
  Particle.function("auto_off", &SpManager::f_auto_off, this);

  Particle.function("schedule", &SpManager::f_create_schedule, this);

  Particle.function("fac_reset", &SpManager::f_factory_reset, this);
  set_skuid_variable();

  switch_off_all();
#if _DEBUG
  Serial.printf("\nSpManager.init: Initialization Finished.");
#endif
}

void SpManager::persist_data_eeprom(int index) {
#if _DEBUG
  Serial.printf(
      "\nSpManager::persist_data_eeprom: Persisting data in EEPROM, %d", index);
#endif

  /*EEPROM.put(SMART_LOAD_EEPROM_STARTING_LOCATION +
                 (index * SMART_LOAD_EEPROM_SIZE),
             *smartLoads[index]);*/
}

void SpManager::persist_data_eeprom() {
  for (int i = 0; i < NUMBER_OF_PINS; i++) {
    persist_data_eeprom(i);
  }
}

int SpManager::f_factory_reset(String t_str) {
  EEPROM.clear();
  return 0;
}
/*
t_str - <pin_number>[:seconds]
<pin_number> - 0 to MAX_SUPPORTED_PINS
*/
int SpManager::f_switch_on(String t_str) {
#if _DEBUG
  Serial.printf("\nf_switch_on: Received '%s'", t_str.c_str());
#endif

  std::string s = t_str.c_str();
  size_t pos = 0;
  std::string token;

  char *endptr;
  int pin = -100, seconds = 0;

  int iterCounter = 0;
  while ((pos = s.find(CLOUD_FN_ARG_DELIM)) != std::string::npos) {
    token = s.substr(0, pos);
    switch (iterCounter) {
    case 0:
#if _DEBUG
      Serial.printf("\nf_switch_on: In Case Pin, %s", token.c_str());
#endif
      pin = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && pin == 0) || (endptr == token.c_str())) {
#if _DEBUG
        Serial.printf("\nf_switch_on: Couldn't find mandatory 'pin' argument");
#endif
        /*return -1;*/
        /*TODO: Just for testing, after that we need to return -1*/
        pin = 0;
      }
      if (pin >= NUMBER_OF_PINS) {
/*return -2;*/
/*TODO: Just for testing, after that we need to return -2*/
#if _DEBUG
        Serial.printf("\nf_switch_on: Pin (%d) greater than supported (%d), "
                      "resetting to 0",
                      pin, NUMBER_OF_PINS);
#endif
        pin = 0;
      }
      break;
    case 1:
#if _DEBUG
      Serial.printf("\nf_switch_on: In Case Seconds, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        seconds = 0;
      }

      seconds = _time;
      break;
    }
    s.erase(0, pos + 1);
    iterCounter++;
  }

  if (pin < 0 || pin >= NUMBER_OF_PINS) {
    return -4;
  }
  if (seconds < 0 || seconds > MAX_ONE_TIME_ON_OFF_TIMER) {
    return -3;
  }

#if _DEBUG
  Serial.printf("\nf_switch_on: PIN: %d, Sec: %d", pin, seconds);
#endif
  smartLoads[pin]->switch_on(seconds);

  return 0;
}

/*
t_str - <pin_number>[:seconds]
<pin_number> - 0 to MAX_SUPPORTED_PINS
*/
int SpManager::f_switch_off(String t_str) {
#if _DEBUG
  Serial.printf("\nf_switch_off: Received '%s'", t_str.c_str());
#endif
  std::string s = t_str.c_str();
  size_t pos = 0;
  std::string token;

  char *endptr;
  int pin = -100, seconds = 0;

  int iterCounter = 0;
  while ((pos = s.find(CLOUD_FN_ARG_DELIM)) != std::string::npos) {
    token = s.substr(0, pos);
    switch (iterCounter) {
    case 0:
#if _DEBUG
      Serial.printf("\nf_switch_off: In Case Pin, %s", token.c_str());
#endif
      pin = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && pin == 0) || (endptr == token.c_str())) {
#if _DEBUG
        Serial.printf("\nf_switch_off: Couldn't find mandatory 'pin' argument");
#endif
        /*return -1;*/
        /*TODO: Just for testing, after that we need to return -1*/
        pin = 0;
      }
      if (pin >= NUMBER_OF_PINS) {
        /*return -2;*/
        /*TODO: Just for testing, after that we need to return -2*/
        pin = 0;
      }
      break;
    case 1:
#if _DEBUG
      Serial.printf("\nf_switch_off: In Case Seconds, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        seconds = 0;
      }

      seconds = _time;
      break;
    }
    s.erase(0, pos + 1);
    iterCounter++;
  }

  if (pin < 0 || pin >= NUMBER_OF_PINS) {
    return -4;
  }
  if (seconds < 0 || seconds > MAX_ONE_TIME_ON_OFF_TIMER) {
    return -3;
  }

  smartLoads[pin]->switch_off(seconds);

  return 0;
}

int SpManager::f_is_switch_on(String t_str) {
#if _DEBUG
  Serial.printf("\nf_switch_off: Received '%s'", t_str.c_str());
#endif
  char *endptr;
  int pin = (int)strtol(t_str.c_str(), &endptr, 10);

  if ((errno != 0 && pin == 0) || (endptr == t_str.c_str())) {
#if _DEBUG
    Serial.printf("\nf_is_switch_on: Couldn't find mandatory 'pin' argument");
#endif
    /*return -1;*/
    /*TODO: Just for testing, after that we need to return -1*/
    pin = 0;
  }
  if (pin < 0 || pin >= NUMBER_OF_PINS) {
    return -2;
  }
  return smartLoads[pin]->is_switch_on() ? 1 : 0;
}

int SpManager::f_auto_off(String t_str) {

#if _DEBUG
  Serial.printf("\nf_auto_off: Received '%s'", t_str.c_str());
#endif

  std::string s = t_str.c_str();
  size_t pos = 0;
  std::string token;

  char *endptr;
  int pin = -100, minutes = 0;

  int iterCounter = 0;
  while ((pos = s.find(CLOUD_FN_ARG_DELIM)) != std::string::npos) {
    token = s.substr(0, pos);
    switch (iterCounter) {
    case 0:

#if _DEBUG
      Serial.printf("\nf_auto_off: In Case Pin, %s", token.c_str());
#endif

      pin = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && pin == 0) || (endptr == token.c_str())) {

#if _DEBUG
        Serial.printf("\nf_auto_off: Couldn't find mandatory 'pin' argument");
#endif
        /*return -1;*/
        /*TODO: Just for testing, after that we need to return -1*/
        pin = 0;
      }
      if (pin >= NUMBER_OF_PINS) {
        /*return -2;*/
        /*TODO: Just for testing, after that we need to return -2*/
        pin = 0;
      }
      break;
    case 1:
#if _DEBUG
      Serial.printf("\nf_auto_off: In Case Seconds, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        minutes = 0;
      }

      minutes = _time;
      break;
    }
    s.erase(0, pos + 1);
    iterCounter++;
  }

  if (pin < 0 || pin >= NUMBER_OF_PINS) {
    return -4;
  }
  if (minutes < 0 || minutes > MAX_AUTO_OFF_ON_TIME) {
    return -3;
  }

  smartLoads[pin]->set_auto_off_time(minutes);
  smartLoads[pin]->start_auto_off();

  persist_data_eeprom(pin);
  return 0;
}

int SpManager::f_auto_on(String t_str) {
#if _DEBUG
  Serial.printf("\nf_auto_on: Received '%s'", t_str.c_str());
#endif
  std::string s = t_str.c_str();
  size_t pos = 0;
  std::string token;

  char *endptr;
  int pin = -100, minutes = 0;

  int iterCounter = 0;
  while ((pos = s.find(CLOUD_FN_ARG_DELIM)) != std::string::npos) {
    token = s.substr(0, pos);
    switch (iterCounter) {
    case 0:
#if _DEBUG
      Serial.printf("\nf_auto_on: In Case Pin, %s", token.c_str());
#endif
      pin = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && pin == 0) || (endptr == token.c_str())) {
#if _DEBUG
        Serial.printf("\nf_auto_on: Couldn't find mandatory 'pin' argument");
#endif
        /*return -1;*/
        /*TODO: Just for testing, after that we need to return -1*/
        pin = 0;
      }
      if (pin >= NUMBER_OF_PINS) {
        /*return -2;*/
        /*TODO: Just for testing, after that we need to return -2*/
        pin = 0;
      }
      break;
    case 1:
#if _DEBUG
      Serial.printf("\nf_auto_on: In Case Seconds, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        minutes = 0;
      }

      minutes = _time;
      break;
    }
    s.erase(0, pos + 1);
    iterCounter++;
  }

  if (pin < 0 || pin >= NUMBER_OF_PINS) {
    return -4;
  }
  if (minutes < 0 || minutes > MAX_AUTO_OFF_ON_TIME) {
    return -3;
  }

  smartLoads[pin]->set_auto_on_time(minutes);
  smartLoads[pin]->start_auto_on();

  persist_data_eeprom(pin);
  return 0;
}

// pin:s_hour:s_min:s_dow:e_hour:e_min:action
int SpManager::f_create_schedule(String t_str) {
#if _DEBUG
  Serial.printf("\nf_create_schedule: Received '%s'", t_str.c_str());
#endif
  std::string s = t_str.c_str();
  size_t pos = 0;
  std::string token;

  char *endptr;
  int pin = -100, s_min = -1, s_hour = -1, e_min = -1, e_hour = -1, s_dow = 0;
  int action = 0; // 0 - OFF, 1 - ON

  int iterCounter = 0;
  while ((pos = s.find(CLOUD_FN_ARG_DELIM)) != std::string::npos) {
    token = s.substr(0, pos);
    switch (iterCounter) {
    case 0: {
#if _DEBUG
      Serial.printf("\nf_create_schedule: In Case Pin, %s", token.c_str());
#endif
      pin = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && pin == 0) || (endptr == token.c_str())) {
#if _DEBUG
        Serial.printf(
            "\nf_create_schedule: Couldn't find mandatory 'pin' argument");
#endif
        /*return -1;*/
        /*TODO: Just for testing, after that we need to return -1*/
        pin = 0;
      }
      if (pin >= NUMBER_OF_PINS) {
        /*return -2;*/
        /*TODO: Just for testing, after that we need to return -2*/
        pin = 0;
      }
    } break;
    case 1: {
#if _DEBUG
      Serial.printf("\nf_create_schedule: In Case s_hour, %s", token.c_str());
#endif
      int _time = 0;
      _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        s_hour = 0;
      }

      s_hour = _time;
    } break;
    case 2: {
#if _DEBUG
      Serial.printf("\nf_create_schedule: In Case s_min, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        s_min = 0;
      }

      s_min = _time;
    } break;
    case 3: {
#if _DEBUG
      Serial.printf("\nf_create_schedule: In Case s_dow, %s", token.c_str());
#endif
      uint8_t _time1 = 0;
      _time1 = (uint8_t)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time1 == 0) || (endptr == token.c_str())) {
        s_dow = 0;
      }

      s_dow = _time1;
      s_dow &= 0xEF; // TO clear MSB as it is of not use and all other checks
      // work well
    } break;
    case 4: {
#if _DEBUG
      Serial.printf("\nf_create_schedule: In Case e_hour, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        e_hour = 0;
      }

      e_hour = _time;
    } break;
    case 5: {
#if _DEBUG
      Serial.printf("\nf_create_schedule: In Case e_min, %s", token.c_str());
#endif
      int _time = (int)strtol(token.c_str(), &endptr, 10);
      if ((errno != 0 && _time == 0) || (endptr == token.c_str())) {
        e_min = 0;
      }

      e_min = _time;
    } break;
    }
    s.erase(0, pos + 1);
    iterCounter++;
  }

  if (pin < 0 || pin >= NUMBER_OF_PINS) {
    return -4;
  }

  if (s_min < 0 || s_min > 59 || e_min < 0 || e_min > 59 || s_hour < 0 ||
      s_hour > 23) {
    return -3;
  }

  Schedule *schedule = new Schedule;
  schedule->s_hour = s_hour;
  schedule->s_min = s_min;
  schedule->s_day_of_week = s_dow;
  schedule->e_hour = e_hour;
  schedule->e_min = e_min;
  schedule->action = ACTION_ON;

  smartLoads[pin]->createSchedule(schedule);

  persist_data_eeprom(pin);
  return 0;
}
