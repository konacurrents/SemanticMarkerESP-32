#ifndef KeyUnitSensorClass_h
#define KeyUnitSensorClass_h

#include "SensorClassType.h"
#include <FastLED.h>

//!@see https://forum.arduino.cc/t/undefined-reference-to-vtable-for-abstract-classname-even-after-derived-class-definition/1084221/6

class KeyUnitSensorClass : public SensorClassType
{
public:
	 //! constructor
    KeyUnitSensorClass(char *config);

//! destructor
    ~KeyUnitSensorClass();

    //! Pure Virtual Function
    void loop();
    
    //! Pure Virtual Function
    void setup();
    
    
private:
    void keyUnitLED();
    void changeLedColor();
    
    //!setup the KeyUnit
    void setupKeyUnit();
    
    //! loop the key unit (after other called M5.updfate)
    void loopKeyUnit();

    //! static wrapper class
    static void startTaskImpl(void*);

    //! these are not initialized...
    int _scotty = 100;
    uint8_t _ledColor = 0;
    CRGB *_leds;        // Define the array of leds.  定义LED阵列.
};

#endif
