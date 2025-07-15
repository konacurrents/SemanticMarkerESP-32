//
//  M5Atom_HDriverModuleClass.cpp
//  
//
//  Created by Scott Moody on 5/6/25.
//

#include "M5Atom_HDriverModuleClass.h"

//! Currently the HDriver only processes the BUZZER .. so the setup works..
M5Atom_HDriverModuleClass::M5Atom_HDriverModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_HDriverModuleClass init %s\n", config);
    
}

//!6.6.24 D-Day 81 years
//!return true or false if this should be a PTFeeder or PTClicker
//!default 'false' to PTClicker.   The Bluetooth will use this
boolean M5Atom_HDriverModuleClass::isPTFeeder_M5AtomClassType()
{
    return true;
}

//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_HDriverModuleClass::start_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_HDriverModuleClass::setup_M5AtomClassType()
{
    SerialDebug.printf("M5Atom_HDriverModuleClass::setup_M5AtomClassType\n");

}

//! stops motor
void M5Atom_HDriverModuleClass::stop_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_HDriverModuleClass::loop_M5AtomClassType()
{
}

//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "HDriver"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_HDriverModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_HDriverModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
    SerialDebug.printf("M5Atom_HDriverModuleClass::messageSend_M5AtomClassType(%s)\n", sendValue);

}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_HDriverModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    
}
//!long press on buttonA (top button)
void M5Atom_HDriverModuleClass::buttonA_LongPress_M5AtomClassType()
{
    
}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_HDriverModuleClass::currentStatusURL_M5AtomClassType()
{
    return (char*)"s=HDriver_TODO";
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_HDriverModuleClass::currentStatusJSON_M5AtomClassType()
{
    return (char*)"hdriverTODO";
}
