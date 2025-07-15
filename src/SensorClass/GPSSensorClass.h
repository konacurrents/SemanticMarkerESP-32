


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
#ifdef NOT_USED
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
    
};
#endif // USE_GPS_SENSOR_CLASS

#endif
