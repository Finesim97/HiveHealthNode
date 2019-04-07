#if !defined _SETUPSERVER
#define _SETUPSERVER
#define NUMBERS "numbers"
#define STRINGS "strings"
#define BOOLS "booleans"
#define SENSORNAME "sensors"

#include "ArduinoJson.h"
#include <Preferences.h>

void getCurrentConfigJSON(Preferences &p);
boolean setConfigJSON(Preferences &p, char* jsonstr);

#endif
