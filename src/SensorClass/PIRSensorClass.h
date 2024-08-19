

#ifndef PIRSensorClass_h
#define PIRSensorClass_h

#include "../../Defines.h"
#ifdef ESP_M5

#include "SensorClassType.h"

class PIRSensorClass : public SensorClassType
{
public:
	 //! constructor
    PIRSensorClass(char *config);

    //! Pure Virtual Function
    void loop();
    
    //! Pure Virtual Function
    void setup();
    
};
#endif

#endif
