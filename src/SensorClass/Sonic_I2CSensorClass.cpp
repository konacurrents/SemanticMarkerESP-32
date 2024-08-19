

#include "Sonic_I2CSensorClass.h"

#include "../../defines.h"
#if defined(ESP_M5) && !defined(ESP_M5_MINIMAL_SENSORS)
//#include <iostream>
#include <string.h>
#include <stdio.h>

#ifdef ESP_M5
#ifdef M5CORE2_MODULE
#include <M5Display.h>
#else

#ifdef M5STICKCPLUS2
#include <M5StickCPlus.h>
#endif

#include <Unit_Sonic.h>
//!@see https://github.com/m5stack/M5Unit-Sonic/blob/master/examples/Unit_SonicI2C_M5StickCPlus/Unit_SonicI2C_M5StickCPlus.ino
#endif
#endif

Sonic_I2CSensorClass::Sonic_I2CSensorClass(char *config)  : SensorClassType(config)
{
    
    printf("Sonic_I2CSensorClass init %s\n", config);
    
}

Sonic_I2CSensorClass::~Sonic_I2CSensorClass()
{
    printf("Sonic_I2CSensorClass destroy\n");
}




//#define USE_LED_BREATH  //not working, (working for M5button and Core2)
//! for ATOM (maybe change the priority and see what happens)


//!wrapper static method
void Sonic_I2CSensorClass::startTaskImpl(void* _this)
{
    SerialDebug.printf(" startTaskImpl == %p\n", _this);
    
}
                   //!setup the Sonic_I2C
void Sonic_I2CSensorClass::setupSonic_I2C()
{
    SerialDebug.printf(" setupSonic_I2C == %p\n", this);
    
    //!start the sensor
    _sensor.begin();
}


//! loop the key unit (after other called M5.updfate)
void Sonic_I2CSensorClass::loopSonic_I2C()
{
    static float sensorValue = 0;
    sensorValue = _sensor.getDistance();
    if (sensorValue < 3000 && sensorValue > 20)
    {
        
        SerialDebug.printf("Sensor Value %f\n", sensorValue);
        
        //!call the callback VALUE
        callCallbackValue((char*)"sonic",(int)sensorValue);

    }
}




//! error: https://stackoverflow.com/questions/3065154/undefined-reference-to-vtable
//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void Sonic_I2CSensorClass::loop()
{
    //!setup the Sonic_I2C
    loopSonic_I2C();
    
    //delay(100);
}

void Sonic_I2CSensorClass::setup()
{
    printf("Sonic_I2CSensorClass::setup()\n");
    
    setupSonic_I2C();
}
#endif
