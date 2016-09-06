#ifndef  __VOODOO_H_
#define  __VOODOO_H_

#include "application.h"

#define   PORT                        48879
#define   MAX_DATA_BYTES              128

// table of action codes
// to do: make this an enum?
#define   KEEPALIVE                   0x00
#define   SWITCH_ON                   0x01
#define   SWITCH_OFF                  0x02
/* GAP */
#define   AUTO_ON_TIME                0x06
#define   AUTO_OFF_TIME               0x07

#define   CREATE_ALARM                0x0B

#define   ACTION_RANGE                0x0F

// Number of Seconds not received keepalives takeoff client
#define   KEEPALIVE_TIMEOUT           15 //Seconds

#define   IS_PHOTON()                 PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION || \
                                      PLATFORM_ID == PLATFORM_P1

#define   IS_CORE()                   PLATFORM_ID == PLATFORM_SPARK_CORE || \
                                      PLATFORM_ID == PLATFORM_SPARK_CORE_HD

void      _setup();
void      _loop();

extern    Timer t_voodootimer;

#endif
