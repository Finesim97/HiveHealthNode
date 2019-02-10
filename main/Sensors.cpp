#include "Sensors.h"


boolean measureMilis(char* strbuffer) {
  sprintf(strbuffer, "%d", millis());
  return true;
}

SensorReading sensors[] = {{"timesensor", true, measureMilis, 10}};
