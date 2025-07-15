
#ifndef Sonic_I2CSensorClass_h
#define Sonic_I2CSensorClass_h
#include "../../Defines.h"
#ifdef ESP_M5

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
#ifdef NOT_DEFINED

    //! adding the messages as well
    //! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
    //! 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the set,val
    //! 12.27.23 support setName == "socket"
    //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
    //! It's up to the receiver to decide if it has to be specified
    void messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified);
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the  send -
    void messageSend_SensorClassType(char *sendValue);
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the cmd
    void messageCmd_SensorClassType(char *cmdValue);
#endif
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

#endif
