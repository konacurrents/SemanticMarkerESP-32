//
//  ChainButtonClassType.hpp
//  ESP_IOT
//
//  Created by Scott Moody on 12/19/25.
//

#ifndef ChainButtonClassType_hpp
#define ChainButtonClassType_hpp

#include "../../Defines.h"

#include "SensorClassType.h"

//! 12.19.25 Add the
//! 12.17.25 No Power. 1 tree down, HIGH winds last night, no flooding here, but valleys and roads bad
//! extend the parser for different SensorKindEnum with different number of parameters and types (int vs str)
//! There will be a setSensors (existing), and then a setM5Chain the new one.. it's own EPROM too
//!the SensorKindEnum is what state we are holding. There will be state models behind each of these.

class ChainButtonClass : public SensorClassType
{
public:
    //! constructor
    ChainButtonClass(char *config);
    
    //! Pure Virtual Function
    void loop();
    
    //! Pure Virtual Function
    void setup();
    
    //! these are not initialized...
    
    //! 12.19.25 add ability to know which kind of SensorClass we are
    //! @see https://stackoverflow.com/questions/307765/how-do-i-check-if-an-objects-type-is-a-particular-subclass-in-c
    //boolean isChainClass = true;
    
    // Define the array of leds.  定义LED阵列.
    //! adding the messages as well
    //! 5.14.25 (Laura/Paul flying). 5.14.74 great Dead
    //! 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the set,val
    //! 12.27.23 support setName == "socket"
    //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
    //! It's up to the receiver to decide if it has to be specified
    void messageSetVal_SensorClassType(char *setName, char* valValue, boolean deviceNameSpecified);
    
#ifdef NOT_DEFINED

    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the  send -
    void messageSend_SensorClassType(char *sendValue);
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the cmd
    void messageCmd_SensorClassType(char *cmdValue);
#endif
    //! 5.15.25 try a special command local to this class
    void messageLocal_SensorClassType(char *message);
    
private:
   
};

#endif /* ChainButtonClassType_hpp */
