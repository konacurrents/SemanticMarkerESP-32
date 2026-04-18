
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

//! 11.27.25 bringing in the M5 code for the HAT
//!
//#define PIR_PIN 36 //passive IR Hat
const uint32_t SLEEP_DURATION_PIRSensorClass = 1 * 1000000;

//unsigned long _PrevSampleTime_PIRSensorClass    = 0;
//unsigned long _PrevTriggerTime_PIRSensorClass    = 0;
//unsigned long _InactivityTimeOut_PIRSensorClass = 0;

#define Elapsed3secs  3000
#define Elapsed4secs  4000

#define Elapsed3mins  180000 // 3 minutes in milliseconds

//!Return "true" on PIR (over sensitivity) and false otherwise
//!This is just a sensor (if plugged in) - so any timing is on the caller of this
bool PIRSensorClass::checkPIR_ButtonProcessing()
{
    //    if (!getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_PIR_VALUE))
    //        return false;
    
    boolean triggeredPIR = digitalRead(_pin2);
    if (triggeredPIR)
    {
        if ((millis()-_PrevTriggerTime_PIRSensorClass)>Elapsed4secs)
        {
            triggeredPIR = true;
            _PrevTriggerTime_PIRSensorClass = millis();
            SerialTemp.println("triggeredPIR = true");
        }
        else
            triggeredPIR = false;
    }
    return triggeredPIR;
}

//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void PIRSensorClass::loop()
{
    //printf("PIRSensorClass::loop()\n");
    
    //! call the check
    long triggered = checkPIR_ButtonProcessing();
    //! what to do with trigger?
    //! call feed
    if (triggered)
        //! 11.26.25 send status as well .. so we can hear what's happening
        //! 8.16.25 MQTT
        //!also send a #STATUS
        sendMessageMQTT((char*)"#STATUS");
}

void PIRSensorClass::setup()
{
    SerialDebug.printf("PIRSensorClass::setup(%d,%d)\n",_pin1, _pin2);

    
    if (_pin1 == 0)
    {
        //! default to 21,33
        //! PIRSensorClass,23,33
        _pin1 = 23;
        _pin2 = 33;
    }
    
    //https://www.reddit.com/r/arduino/comments/1g89dlo/esp32_crashing_due_to_pinmode_and_fastled/
//    pinMode(_pin1, OUTPUT);
//    pinMode(_pin2, OUTPUT);
    pinMode(_pin2,INPUT_PULLUP);
    
    registerPinUse_mainModule(_pin1,"Pin1","PIRSensorClass",false);
    registerPinUse_mainModule(_pin2,"Pin2","PIRSensorClass",false);
    
    //! turn off..
//    digitalWrite(_pin1, LOW);
//    digitalWrite(_pin2, LOW);
    
    SerialDebug.println("DONE PIRSensorClass::setup");
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
