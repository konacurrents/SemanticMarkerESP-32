//
//  M5Atom_Core2ModuleClass.cpp
//  
//
//  Created by Scott Moody on 5/9/25.
//

#include "M5Atom_Core2ModuleClass.h"

#include "../M5Core2Module/M5Core2Module.h"

M5Atom_Core2ModuleClass::M5Atom_Core2ModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_Core2ModuleClass init %s\n", config);
    
}


//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_Core2ModuleClass::start_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_Core2ModuleClass::setup_M5AtomClassType()
{
    ///! 7.31.25 oops.. was Atom
    setup_M5Core2Module();
}

//! stops motor
void M5Atom_Core2ModuleClass::stop_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_Core2ModuleClass::loop_M5AtomClassType()
{
    ///! 7.31.25 oops.. was Atom
    loop_M5Core2Module();
}

//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "QRCode"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_Core2ModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    messageSetVal_M5Core2Module(setName, valValue, deviceNameSpecified);
    
}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_Core2ModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
    
}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_Core2ModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    buttonA_ShortPress_M5Core2Module();
    
}
//!long press on buttonA (top button)
void M5Atom_Core2ModuleClass::buttonA_LongPress_M5AtomClassType()
{
    buttonA_LongPress_M5Core2Module();
    
}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_Core2ModuleClass::currentStatusURL_M5AtomClassType()
{
    return currentStatusURL_M5Core2Module();
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_Core2ModuleClass::currentStatusJSON_M5AtomClassType()
{
    return currentStatusJSON_M5Core2Module();
}

