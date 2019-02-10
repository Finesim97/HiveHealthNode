#if !defined _SENSORSDEF
#define _SENSORSDEF

#include <Arduino.h>

#include "MQTTService.h"

boolean measureMilis(char* strbuffer);

#define SENSORS 1
extern SensorReading sensors[];

#endif
