


#ifndef GPSSensorClass_h
#define GPSSensorClass_h

#include "../../Defines.h"
#ifdef USE_GPS_SENSOR_CLASS

#include "SensorClassType.h"

class GPSSensorClass : public SensorClassType
{
public:
    //! constructor
    GPSSensorClass(char *config);
    
    //! Pure Virtual Function
    void loop();
    
    //! Pure Virtual Function
    void setup();
    
    //! these are not initialized...
   
    // Define the array of leds.  定义LED阵列.
};
#endif // USE_GPS_SENSOR_CLASS

#endif
