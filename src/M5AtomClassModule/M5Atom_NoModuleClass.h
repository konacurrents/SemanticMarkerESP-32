//
//  M5Atom_NoModuleClass.hpp
//  ESP_IOT
//
//  Created by Scott Moody on 10/26/25.
//

#ifndef M5Atom_NoModuleClass_hpp
#define M5Atom_NoModuleClass_hpp



#include "../../Defines.h"

#include "M5AtomClassType.h"

//! An concrete class
class M5Atom_NoModuleClass : public M5AtomClassType
{
public:
    //! constructor
    M5Atom_NoModuleClass(char *config);
    
    //! destructor
    ~M5Atom_NoModuleClass();
    
    //! Data members of class
    //! Generic config info .. specific the the sensor type
    
    //!starts the PTStepper
    void start_M5AtomClassType();
    
    //!setup the PTStepper
    void setup_M5AtomClassType();
    
    //! stops motor
    void stop_M5AtomClassType();
    
    //!setup the PTStepper
    void loop_M5AtomClassType();
    
    //! ATOM specific
    
    //! these are from the ATOM
    
    //! 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the set,val
    //! 12.27.23 support setName == "HDriver"
    //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
    //! It's up to the receiver to decide if it has to be specified
    void messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified);
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the  send -
    void messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified);
    
    //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
    //! for the cmd
    //void messageCmd_M5AtomClassType(char *cmdValue);
    
    //! BUTTON PROCESSING abstraction
    //!short press on buttonA (top button)
    void buttonA_ShortPress_M5AtomClassType();
    //!long press on buttonA (top button)
    void buttonA_LongPress_M5AtomClassType();
    
    //!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
    //!starts with "&"*
    char * currentStatusURL_M5AtomClassType();
    
    //!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
    //!starts with "&"*
    char * currentStatusJSON_M5AtomClassType();
    
    
    //!6.6.24 D-Day 81 years
    //!return true or false if this should be a PTFeeder or PTClicker
    //!default 'false' to PTClicker.   The Bluetooth will use this
    boolean isPTFeeder_M5AtomClassType();
    
private:
    //! display SerialDebug
    void displayInfo_No();
    
};

#endif /* M5Atom_NoModuleClass_hpp */

