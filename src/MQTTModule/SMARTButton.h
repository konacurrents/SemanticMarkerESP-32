#ifndef SMARTButton_h
#define SMARTButton_h
#include "../../Defines.h"

#ifdef PROCESS_SMART_BUTTON_JSON
//! 3.3.22 Using the new JSON library which is supposed to catch syntax errors without blowing up
//https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
#include <ArduinoJson.h>

//! parses and stores a SMARTButton
boolean processJSONSMARTButton(DynamicJsonDocument smartJSONObject);
#endif
#endif
