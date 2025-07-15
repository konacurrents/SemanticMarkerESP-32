//
//  M5Atom_SocketModuleClass.cpp
//  
//
//  Created by Scott Moody on 5/6/25.
//

#include "M5Atom_SocketModuleClass.h"

#include "../ATOM_SocketModule/ATOM_SocketModule.h"

M5Atom_SocketModuleClass::M5Atom_SocketModuleClass(char *config) : M5AtomClassType(config)
{
    SerialDebug.printf("M5Atom_SocketModuleClass init %s\n", config);
    
}


//! Data members of class
//! Generic config info .. specific the the sensor type

//!starts the PTStepper
void M5Atom_SocketModuleClass::start_M5AtomClassType()
{
}

//!setup the PTStepper
void M5Atom_SocketModuleClass::setup_M5AtomClassType()
{
    setup_ATOM_SocketModule();
}

//! stops motor
void M5Atom_SocketModuleClass::stop_M5AtomClassType()
{
    
}

//!setup the PTStepper
void M5Atom_SocketModuleClass::loop_M5AtomClassType()
{
    loop_ATOM_SocketModule();
}

//! ATOM specific

//! these are from the ATOM

//! 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the set,val
//! 12.27.23 support setName == "socket"
//! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
//! It's up to the receiver to decide if it has to be specified
void M5Atom_SocketModuleClass::messageSetVal_M5AtomClassType(char *setName, char* valValue, boolean deviceNameSpecified)
{
    messageSetVal_ATOM_SocketModule(setName, valValue, deviceNameSpecified);

}

//! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
//! for the  send -
void M5Atom_SocketModuleClass::messageSend_M5AtomClassType(char *sendValue, boolean deviceNameSpecified)
{
    SerialDebug.printf("M5Atom_SocketModuleClass::messageSend_M5AtomClassType: %s\n", sendValue);
    
    messageSend_ATOM_SocketModule(sendValue);

}

//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void M5Atom_SocketModuleClass::buttonA_ShortPress_M5AtomClassType()
{
    buttonA_ShortPress_ATOM_SocketModule();

}
//!long press on buttonA (top button)
void M5Atom_SocketModuleClass::buttonA_LongPress_M5AtomClassType()
{
    buttonA_LongPress_ATOM_SocketModule();

}

//!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_SocketModuleClass::currentStatusURL_M5AtomClassType()
{
    return currentStatusURL_ATOM_SocketModule();
}

//!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
//!starts with "&"*
char * M5Atom_SocketModuleClass::currentStatusJSON_M5AtomClassType()
{
    return currentStatusJSON_ATOM_SocketModule();
}

/*
 //!THIS IS the setup() and loop() but using the "component" name, eg AudioModule()
 //!This will perform preference initializtion as well
 //! called from the setup()
 //! Pass in the method to call on a loud (over a threshhold. The parameter for value will be sent
 //void setup_AudioModule(void (*loudCallback)(int));
 void setup_ATOM_SocketModule();
 
 //! called for the loop() of this plugin
 void loop_ATOM_SocketModule();
 
 
 //! 8.28.23  Adding a way for others to get informed on messages that arrive
 //! for the set,val
 //! 12.27.23 support setName == "socket"
 //! 1.10.24 if deviceNameSpecified then this matches this device, otherwise for all.
 //! It's up to the receiver to decide if it has to be specified
 void messageSetVal_ATOM_SocketModule(char *setName, char* valValue, boolean deviceNameSpecified);
 
 //! 12.28.23, 8.28.23  Adding a way for others to get informed on messages that arrive
 //! for the set,val
 void messageSend_ATOM_SocketModule(char *sendValue);
 
 //! BUTTON PROCESSING abstraction
 //!short press on buttonA (top button)
 void buttonA_ShortPress_ATOM_SocketModule();
 //!long press on buttonA (top button)
 void buttonA_LongPress_ATOM_SocketModule();
 
 //!returns a string in in URL so:  status&battery=84'&buzzon='off'  } .. etc
 //!starts with "&"*
 char * currentStatusURL_ATOM_SocketModule();
 
 //!returns a string in in JSON so:  status&battery=84'&buzzon='off'  } .. etc
 //!starts with "&"*
 char * currentStatusJSON_ATOM_SocketModule();
 */
