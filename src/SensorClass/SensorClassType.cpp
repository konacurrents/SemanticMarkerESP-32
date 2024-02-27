
#include "SensorClassType.h"

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

//!call the callback
void SensorClassType::callCallbackValue(char *info, int value)
{
    (*_callbackValue)(info,value);
}

