//
//  BuzzerSensorClass.cpp
//  
//
//  Created by Scott Moody on 5/14/25.
//

#include "BuzzerSensorClass.h"


#include "../../Defines.h"

#include "BuzzerSensorClass.h"


BuzzerSensorClass::BuzzerSensorClass(char *config) : SensorClassType(config)
{
    
    printf("BuzzerSensorClass init %s\n", config);
}


//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void BuzzerSensorClass::loop()
{
   
}


void BuzzerSensorClass::setup()
{
    SerialDebug.printf("BuzzerSensorClass::setup(%d,%d)\n",_pin1, _pin2);
    
    if (_pin1 == 0)
    {
        //! default to 21,33
        //! BuzzerSensorClass,23,33
        _pin1 = 23;
        _pin2 = 33;
    }

    //https://www.reddit.com/r/arduino/comments/1g89dlo/esp32_crashing_due_to_pinmode_and_fastled/
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);

    registerPinUse_mainModule(_pin1,"Pin1","BuzzerSensorClass",false);
    registerPinUse_mainModule(_pin2,"Pin2","BuzzerSensorClass",false);

    //! turn off..
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, LOW);
   
    SerialDebug.println("DONE BuzzerSensorClass::setup");
}

/**
 ** PIN_USE: Pin2 = 22, module=BuzzerSensorClass
 ** PIN_USE: RX = 22, module=ATOMQRCodeModule
 ** PIN_USE: TX = 19, module=ATOMQRCodeModule
 ** PIN_USE: DATA_PIN = 27, module=LED_DisPlay::begin
 ** PIN_USE: KEY_PIN = 32, module=KeyUnitSensorClass
 ** PIN_USE: DATA_PIN = 26, module=KeyUnitSensorClass
 ** PIN_USE: FastLED = 26, module=KeyUnitSensorClass
 ** PIN_USE: IN1_PIN = 21, module=L9110S_DCStepperClass
 ** PIN_USE: IN2_PIN = 25, module=L9110S_DCStepperClass
 */

//! local code
//! if using the GROVE, the right 2 pins would be 26, 32
void BuzzerSensorClass::performBuzzer()
{
    SerialDebug.printf("BuzzerSensorClass::performBuzzer(pin1=%d,pin2=%d)\n",_pin1, _pin2);
    digitalWrite(_pin1, HIGH);
    digitalWrite(_pin2, LOW);
    delay(100);
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, LOW);
}

//! 5.15.25 try a special command local to this class
void BuzzerSensorClass::messageLocal_SensorClassType(char *message)
{
    SerialDebug.printf("BuzzerSensorClass::messageLocal_SensorClassType(%s)\n", message);
    if (strcmp(message,"click")==0)
    {
        SerialDebug.println("BuzzerSensorClass.click");
        performBuzzer();
    }
}
#ifdef NOT_DEFINED


//! adding the messages as well
//! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void BuzzerSensorClass::messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void BuzzerSensorClass::messageSend_SensorClassType(char *sendValue)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the cmd
void BuzzerSensorClass::messageCmd_SensorClassType(char *cmdValue)
{
    
}
#endif //NOT_DEFINED
