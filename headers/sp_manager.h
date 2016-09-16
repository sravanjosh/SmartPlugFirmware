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

  SpManager() {}

  int f_switch_off(String);
  int f_switch_on(String);
  int f_is_switch_on(String);

  int f_auto_off(String);
  int f_auto_on(String);

  int f_create_schedule(String);

  int f_factory_reset(String);

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

public:
  SmartLoad *smartLoads[NUMBER_OF_PINS];

  void switch_off_all();
  void switch_on_all();

  void init();

  void persist_data_eeprom();
  void persist_data_eeprom(int);

  static SpManager *instance() {
    if (!spManagerInstance) {
      spManagerInstance = new SpManager;
    }

    return spManagerInstance;
  }
};
