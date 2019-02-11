#include "Sensors.h"


boolean measureMilis(char* strbuffer) {
  sprintf(strbuffer, "%d", millis());
  return true;
}

boolean measureHall(char* strbuffer) {
  sprintf(strbuffer, "%d", hallRead());
  return true;
}


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
SensorReading sensors[] = {{"timesensor", true, measureMilis, 10},{"hall",false,measureHall,50},{"intemp",false,measureT,100}};
