
#ifndef Sonic_I2CSensorClass_h
#define Sonic_I2CSensorClass_h

#include "SensorClassType.h"
#include <Unit_Sonic.h>

//!@see https://forum.arduino.cc/t/undefined-reference-to-vtable-for-abstract-classname-even-after-derived-class-definition/1084221/6

class Sonic_I2CSensorClass : public SensorClassType
{
public:
    //! constructor
    Sonic_I2CSensorClass(char *config);
    
    //! destructor
    ~Sonic_I2CSensorClass();
    
    //! Pure Virtual Function
    void loop();
    
    //! Pure Virtual Function
    void setup();
    
    
private:
    
    //!setup the Sonic_I2C
    void setupSonic_I2C();
    
    //! loop the key unit (after other called M5.updfate)
    void loopSonic_I2C();
    
    //! static wrapper class
    static void startTaskImpl(void*);
    
    //! these are not initialized...
    int _sensorValue = 0;
      
    //! state specific info
    SONIC_I2C _sensor;
};

#endif
