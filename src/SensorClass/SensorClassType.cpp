
#include "SensorClassType.h"
#include "../../Defines.h"
#ifdef ESP_M5

SensorClassType::SensorClassType(char* config)
{
    SerialDebug.printf("SensorClassType init %s\n", config);
    //! 5.3.25 create storage here
    _identityString = (char*)calloc(strlen(config)+1, sizeof(char));
    strcpy(_identityString, config);
}

//! destructor
SensorClassType::~SensorClassType()
{
    SerialDebug.printf("SensorClassType destroy\n");
}


//! returns the identity .. actaully the config
//! This can be used to see if this class instance matches
//! a known name (like DCMotorStepper, etc)
char *SensorClassType::classIdentity()
{
    SerialDebug.printf("SensorClassType::classIdentity = %s\n", _identityString);
    return _identityString;
}


//! the callback when events in the sensorClass occur
//! This is provided on instantiation
void SensorClassType::registerCallback(sensorCallbackSignature *callback)
{
    _callback = callback;
}

//!call the callback
void SensorClassType::callCallback(char *info, boolean flag)
{
    (*_callback)(info,flag);
}

//! the callback when events in the sensorClass occur
//! This is provided on instantiation
void SensorClassType::registerCallbackValue(sensorCallbackValueSignature *callback)
{
    _callbackValue = callback;
}

//! set PIN 1 and PIN 2 .
//! the class instance has to decide what to do with the pin definitions
void SensorClassType::setPinValues(int pin1, int pin2)
{
    _pin1 = pin1;
    _pin2 = pin2;
}

//!call the callback
void SensorClassType::callCallbackValue(char *info, int value)
{
    (*_callbackValue)(info,value);
}


//!call the callback for JSON
void SensorClassType::callCallbackJSON(char *JSONString)
{
    (*_callbackJSON)(JSONString);
}

//! the callback when events in the sensorClass occur
//! This is provided on instantiation
void SensorClassType::registerCallbackJSON(sensorCallbackJSONSignature *callback)
{ 
    //! callback storage for the JSON
    _callbackJSON = callback;
}

#endif
