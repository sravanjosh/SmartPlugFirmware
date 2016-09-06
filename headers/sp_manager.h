#include "device_control.h"
#include "globals.h"

#if SKU_ID == SKU_LINK_PLUG_1
#define NUMBER_OF_PINS 1
#elif SKU_ID == SKU_LINK_PLUG_2
#define NUMBER_OF_PINS 2
#elif SKU_ID == SKU_LINK_PLUG_4
#define NUMBER_OF_PINS 4
#elif SKU_ID == SKU_LINK_PLUG_8
#define NUMBER_OF_PINS 8
#endif

class SpManager {
private:
  static SpManager *spManagerInstance;

  SpManager() {
    for (int i = 0; i < NUMBER_OF_PINS; i++) {
      smartLoads[i].set_pin(SUPPORTED_PINS[i]);
    }

    Particle.function("switch_on", &SpManager::f_switch_on, this);
    Particle.function("switch_off", &SpManager::f_switch_off, this);
    Particle.function("is_switch_on", &SpManager::f_is_switch_on, this);

    Particle.function("auto_on", &SpManager::f_auto_on, this);
    Particle.function("auto_off", &SpManager::f_auto_off, this);

    set_skuid_variable();
  }

  void set_skuid_variable() {
#if SKU_ID == SKU_LINK_PLUG_1
    char *sku_id = "SKU_LINK_PLUG_1";
#elif SKU_ID == SKU_LINK_PLUG_2
    char *sku_id = "SKU_LINK_PLUG_2";
#elif SKU_ID == SKU_LINK_PLUG_4
    char *sku_id = "SKU_LINK_PLUG_4";
#elif SKU_ID == SKU_LINK_PLUG_8
    char *sku_id = "SKU_LINK_PLUG_8";
#endif
    Particle.variable("sku_id", sku_id);
  }

  int f_switch_off(String t_str);
  int f_switch_on(String t_str);
  int f_is_switch_on(String t_str);

  int f_auto_off(String t_str);
  int f_auto_on(String t_str);

public:
  SmartLoad smartLoads[NUMBER_OF_PINS];

  void switch_off_all();
  void switch_on_all();

  void init() {
    switch_off_all();
#if _DEBUG
    Serial.printf("\nSpManager.init: Initialization Finished.");
#endif
  }

  static SpManager *instance() {
    if (!spManagerInstance) {
      spManagerInstance = new SpManager;
    }

    return spManagerInstance;
  }
};
