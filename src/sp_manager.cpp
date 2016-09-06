#include "sp_manager.h"

SpManager *SpManager::spManagerInstance = 0;

void SpManager::switch_on_all() {
  for (SmartLoad &smart_load : smartLoads) {
    smart_load.switch_on();
  }
}

void SpManager::switch_off_all() {
  for (SmartLoad &smart_load : smartLoads) {
    smart_load.switch_off();
  }
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
  smartLoads[pin].switch_on(seconds);

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

  smartLoads[pin].switch_off(seconds);

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
  return smartLoads[pin].is_switch_on() ? 1 : 0;
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

  smartLoads[pin].set_auto_off_time(minutes);
  smartLoads[pin].start_auto_off();

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

  smartLoads[pin].set_auto_on_time(minutes);
  smartLoads[pin].start_auto_on();

  return 0;
}
