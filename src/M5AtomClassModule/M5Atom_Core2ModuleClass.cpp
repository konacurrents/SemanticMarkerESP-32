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
    setup_ATOMQRCodeModule();
}

//! stops motor
void M5Atom_Core2ModuleClass::stop_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_Core2ModuleClass::loop_M5AtomClassType()
{
    loop_ATOMQRCodeModule();
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
/*
 
 //!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
 //!This will perform preference initializtion as well
 //! called from the setup()
 //! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
 //void setup_AudioModule(void (*loudCallback)(int));
 void setup_ATOMQRCodeModule();
 
 //! called for the loop() of this plugin
 void loop_ATOMQRCodeModule();
 
 
 //! 8.28.23  Adding a way for others to get informed on messages that arrive
 //! for the set,val
 //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
 //! It's up to the receiver to decide if it has to be specified
 void messageSetVal_ATOMQRCodeModule(char *setName, char* valValue, boolean deviceNameSpecified);
 
 //! BUTTON PROCESSING abstraction
 //!short press on buttonA (top button)
 void buttonA_ShortPress_ATOMQRCodeModule();
 //!long press on buttonA (top button)
 void buttonA_LongPress_ATOMQRCodeModule();
 
 //!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
 //!//!starts with "&"*
 char * currentStatusURL_ATOMQRCodeModule();
 
 
 //!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
 //!starts with "&"*
 char * currentStatusJSON_ATOMQRCodeModule();
 
 */
