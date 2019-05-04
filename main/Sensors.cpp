#include "Sensors.h"

/*
 * The sensoring methods
 */

// Read the the current run time
boolean measureMilis(char* strbuffer) { 
  sprintf(strbuffer, "%d", millis());
  return true;
}

// Read the internal hall sensor
boolean measureHall(char* strbuffer) {
  sprintf(strbuffer, "%d", hallRead());
  return true;
}

// Needed to use the internal temp sensor
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

boolean measureT(char* strbuffer){
  float t=(temprature_sens_read()-32)*0.555555; 
  sprintf(strbuffer, "%f",t);
  return true;
}


// Remeber to set the SENSORS variable in the header
SensorReading sensors[] = {{"timesensor", true, measureMilis, 100},{"hall",false,measureHall,50},{"intemp",false,measureT,40}};
