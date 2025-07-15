
#include "PIRSensorClass.h"
#include "../../Defines.h"
//! minimal sensors are a sugset of sensors...
#if defined(ESP_M5) && !defined(ESP_M5_MINIMAL_SENSORS)
//#include <iostream>
#include <string.h>
#include <stdio.h>
PIRSensorClass::PIRSensorClass(char *config) : SensorClassType(config)
{

   printf("PIRSensorClass init %s\n", config);
}

//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void PIRSensorClass::loop()
{
   printf("PIRSensorClass::loop()\n");
}

void PIRSensorClass::setup()
{
   printf("PIRSensorClass::setup()\n");
}


#ifdef NOT_USED
//! adding the messages as well
//! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void PIRSensorClass::messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! 5.15.25 try a special command local to this class
void PIRSensorClass::messageLocal_SensorClassType(char *message)
{
    
}
//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void PIRSensorClass::messageSend_SensorClassType(char *sendValue)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the cmd
void PIRSensorClass::messageCmd_SensorClassType(char *cmdValue)
{
    
}
#endif


#endif
